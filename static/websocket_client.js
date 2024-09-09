function decipher(text, key) {
    let ws_conn = new WebSocket("ws://localhost:9000/crypto_ws");
    ws_conn.onmessage = (event) => {
        let reply = JSON.parse(event.data);
        console.log("The plaintext is " + reply.message + " and the key is " + reply.key + ".");
        ws_conn.close();
    };
    ws_conn.onopen = () => {
        ws_conn.send(JSON.stringify({
            cipherType: "caesar",
            message: text,
            key: key
        }));
    }
}

let msg = prompt("Input the ciphertext.");
decipher(msg, "");