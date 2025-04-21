#include"chandle.h"
#include"caesar.h"
#include"viginere.h"
#include<stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

const char* decrypt_caesar(const char *msg, char **key) {
    void *caesar_decipherer = new caesarCipher;
    const char *res = reinterpret_cast<caesarCipher*>(caesar_decipherer)->decrypt(msg, key);
    delete reinterpret_cast<caesarCipher*>(caesar_decipherer);
    return res;
}

const char* decrypt_viginere(const char *msg, char **key) {
    void *caesar_decipherer = new ViginereCipher;
    const char *res = reinterpret_cast<ViginereCipher*>(caesar_decipherer)->decrypt(msg, key);
    delete reinterpret_cast<ViginereCipher*>(caesar_decipherer);
    return res;
}

double compute_stat(const char* msg) {
    BigramAnalyser ba{};
    try {
        return ba.computeTestStatistic(std::string{msg});
    } catch (std::invalid_argument e) {
        return INFINITY;
    }
    
}

#ifdef __cplusplus
}
#endif