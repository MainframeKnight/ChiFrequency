#pragma once


#ifdef __cplusplus
extern "C" {
#endif

const char* decrypt_caesar(const char *msg, char** key);
const char* decrypt_viginere(const char *msg, char** key);

#ifdef __cplusplus
}
#endif