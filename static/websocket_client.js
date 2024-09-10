function decipher(text, key) {
    return new Promise( (resolve, reject) => {
        let ws_conn = new WebSocket("ws://localhost:9000/crypto_ws");
        ws_conn.onmessage = (event) => {
            let reply = JSON.parse(event.data);
            ws_conn.close();
            resolve(reply);

        };
        ws_conn.onopen = () => {
            ws_conn.send(JSON.stringify({
                cipherType: "caesar",
                message: text,
                key: key
            }));
        }
    });
}

document.querySelector("#automatic").onchange = () => {
    document.querySelector("#key").hidden = !document.querySelector("#key").hidden;
}

// The support of numbers in ciphertext should also be added later.
document.querySelector("#decipher_button").onclick = () => {
    let ciphertext = document.querySelector("#ciphertext").value;
    if (!RegExp("[a-zA-Z]").test(ciphertext)) {
        console.log(ciphertext);
        document.querySelector("#result").value = "Ciphertext does not contain any letters.";
        return;
    }
    if (document.querySelector("#automatic").checked) {
        decipher(ciphertext, "").then( (result) => {
            document.querySelector("#result").value = "Plaintext: " + 
                result.message + "\nKey: " + result.key;
        });
    } else {
        decipher(ciphertext, document.querySelector("#key_input").value).then( (result) => {
            document.querySelector("#result").value = "Plaintext: " + result.message;
        });
    }
}