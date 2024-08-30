#include"chandle.h"
#include"caesar.h"
#include<stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* decrypt_caesar(const char *msg) {
    void *caesar_decipherer = new caesarCipher;
    const char *res = reinterpret_cast<caesarCipher*>(caesar_decipherer)->decrypt(msg);
    delete reinterpret_cast<caesarCipher*>(caesar_decipherer);
    return res;
}

#ifdef __cplusplus
}
#endif