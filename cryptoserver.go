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
	"net/http"
	"unsafe"

	"github.com/gorilla/websocket"
)

func decryptCaesar(str string) string {
	p := C.CString(str)
	defer C.free(unsafe.Pointer(p))
	res := C.decrypt_caesar(p)
	defer C.free(unsafe.Pointer(res))
	return C.GoString(res)
}

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
}

type Query struct {
	CipherType string `json:"cipherType"`
	Message    string `json:"message"`
}

func main() {
	http.HandleFunc("/crypto", func(w http.ResponseWriter, r *http.Request) {
		fmt.Printf("%s connected!\n", r.RemoteAddr)
		conn, _ := upgrader.Upgrade(w, r, nil)
		defer conn.Close()
		_, msg, _ := conn.ReadMessage()
		var query Query
		var res string
		fmt.Printf("Received query: %s\n", string(msg[:]))
		err := json.Unmarshal(msg, &query)
		if err != nil {
			fmt.Printf("Bad query structure!\n")
			return
		}
		switch query.CipherType {
		case "caesar":
			res = decryptCaesar(query.Message)
		}
		fmt.Printf("Returning the result: %s\n", res)
		conn.WriteMessage(websocket.TextMessage, []byte(res))
	})
	fmt.Printf("Running the SERVER!")
	err := http.ListenAndServe(":9000", nil)
	if err != nil {
		fmt.Printf("Error: %s\n", err.Error())
	}
}
