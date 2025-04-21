package main

/*
#cgo LDFLAGS: -L${SRCDIR}/lib/linux -L ./ciphers -Wl,-rpath=./ciphers -lcipher
#include <stdlib.h>
#include "ciphers/chandle.h"
*/
import "C"
import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"os"
	"strconv"
	"strings"
	"unicode"
	"unsafe"

	"github.com/gorilla/websocket"
)

func check_first_letters(str string, out *string) {
	eng_lets := "qwertyuioplkjhgfdsazxcvbnm"
	first_lets := ""
	space := false
	for _, i := range str {
		if !space && strings.Contains(eng_lets, strings.ToLower(string(i))) {
			first_lets += string(unicode.ToLower(i))
			space = true
		} else if !space {
			space = true
		} else if space &&
			unicode.IsSpace(i) {
			space = false
		}
	}
	if len(first_lets) < 2 {
		return
	}
	p := C.CString(first_lets)
	defer C.free(unsafe.Pointer(p))
	if C.compute_stat(p) <= 300 {
		*out += "Found text \"" + first_lets + "\" hidden by taking first letters.\n"
	}
}

func zero_width_check(msg string, out *string) {
	secret_data := ""
	for _, i := range msg {
		if i == rune(0x200B) {
			secret_data += "0"
		} else if i == rune(0xFEFF) {
			secret_data += "1"
		}
	}
	if len(secret_data) == 0 {
		return
	}
	if len(secret_data)%8 != 0 {
		*out += "Found zero-width spaces in text (non-aligned as byte array data).\n"
		return
	}
	ascii_bytes := []byte{}
	for i := 0; i < len(secret_data); i += 8 {
		v, _ := strconv.ParseInt(secret_data[i:i+8], 2, 8)
		ascii_bytes = append(ascii_bytes, byte(v))
	}
	res := string(ascii_bytes[:])
	for _, i := range res {
		if !unicode.IsDigit(i) && !unicode.IsLetter(i) && !unicode.IsPunct(i) {
			return
		}
	}
	p := C.CString(res)
	defer C.free(unsafe.Pointer(p))
	if C.compute_stat(p) <= 300 {
		*out += "Found text \"" + res + "\" hidden by using zero-width spaces.\n"
	} else {
		*out += "Found zero-width spaces in text.\n"
	}
}

func susp_char_check(msg string, out *string) {
	eng_lets := "qwertyuioplkjhgfdsazxcvbnm"
	punct := "~!@#$%^&*()-+=_`{}[]\\|\"':;<>,./? \t"
	is_start, is_space, found_start, found_newl := true, false, false, false
	for pos, i := range msg {
		if unicode.IsSpace(i) && is_start && !found_start {
			found_start = true
		} else if i == '\n' && is_space && !found_newl || pos == len(msg)-1 && unicode.IsSpace(i) {
			found_newl = true
		}
		if !unicode.IsDigit(i) && !strings.Contains(eng_lets, strings.ToLower(string(i))) &&
			!strings.Contains(punct, strings.ToLower(string(i))) && i != 0x200B && i != 0xFEFF {
			*out += "Found a suspicious symbol with unicode code " + strconv.Itoa(int(i)) + ".\n"
		}
		is_space = unicode.IsSpace(i) && i != '\n'
		is_start = i == '\n'
	}
	if found_start {
		*out += "Found line starting by whitespace.\n"
	}
	if found_newl {
		*out += "Found line ending by whitespace.\n"
	}
}

func steganalysis(str string) string {
	res := ""
	check_first_letters(str, &res)
	zero_width_check(str, &res)
	susp_char_check(str, &res)
	if len(res) == 0 {
		res += "No secrets found in text."
	}
	return res
}

func decrypt(str string, key string, cipher string) (string, string) {
	p := C.CString(str)
	defer C.free(unsafe.Pointer(p))
	var k *C.char
	if key == "" {
		k = nil
	} else {
		k = C.CString(key)
	}
	defer C.free(unsafe.Pointer(k))
	var res *C.char
	switch cipher {
	case "caesar":
		res = C.decrypt_caesar(p, &k)
	case "viginere":
		res = C.decrypt_viginere(p, &k)
	case "steg":
		res0 := steganalysis(str)
		return res0, ""
	}
	defer C.free(unsafe.Pointer(res))
	return C.GoString(res), C.GoString(k)
}

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

type Data struct {
	CipherType string `json:"cipherType"`
	Message    string `json:"message"`
	Key        string `json:"key"`
}

func main() {
	http.HandleFunc("/crypto_ws", func(w http.ResponseWriter, r *http.Request) {
		fmt.Printf("%s connected using websockets!\n", r.RemoteAddr)
		conn, _ := upgrader.Upgrade(w, r, nil)
		defer conn.Close()
		_, msg, _ := conn.ReadMessage()
		var query Data
		var res string
		var key_res string
		fmt.Printf("Received data: %s\n", string(msg[:]))
		err := json.Unmarshal(msg, &query)
		if err != nil {
			fmt.Printf("Bad query structure!\n")
			return
		}
		res, key_res = decrypt(query.Message, query.Key, query.CipherType)
		fmt.Printf("Returning the result: %s, key = %s\n", res, key_res)
		var result Data
		result.Key = key_res
		result.Message = res
		result.CipherType = query.CipherType
		conn.WriteJSON(result)
	})
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Printf("%s connected to mainpage!\n", r.RemoteAddr)
		html_file, _ := os.Open("index.html")
		defer html_file.Close()
		io.Copy(w, html_file)
	})
	http.Handle("/static/", http.StripPrefix("/static/", http.FileServer(http.Dir("static"))))
	fmt.Printf("Running the SERVER!")
	err := http.ListenAndServe(":9000", nil)
	if err != nil {
		fmt.Printf("Error: %s\n", err.Error())
	}
}
