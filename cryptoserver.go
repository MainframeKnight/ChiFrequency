package main

/*
#cgo LDFLAGS: -L${SRCDIR}/lib/linux -L ./ciphers -Wl,-rpath=./ciphers -lcaesar
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
	"unsafe"

	"github.com/gorilla/websocket"
)

func decryptCaesar(str string, key string) (string, string) {
	p, k := C.CString(str), C.CString(key)
	defer C.free(unsafe.Pointer(p))
	defer C.free(unsafe.Pointer(k))
	res := C.decrypt_caesar(p, &k)
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
		switch query.CipherType {
		case "caesar":
			res, key_res = decryptCaesar(query.Message, query.Key)
		}
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
	fmt.Printf("Running the SERVER!")
	err := http.ListenAndServe(":9000", nil)
	if err != nil {
		fmt.Printf("Error: %s\n", err.Error())
	}
}
