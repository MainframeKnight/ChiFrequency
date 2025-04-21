#pragma once


#ifdef __cplusplus
extern "C" {
#endif

const char* decrypt_caesar(const char *msg, char** key);
const char* decrypt_viginere(const char *msg, char** key);
double compute_stat(const char *msg);

#ifdef __cplusplus
}
#endif