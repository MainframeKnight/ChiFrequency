#include<stdio.h>
#include<stdlib.h>
#include"chandle.h"
int main() {
    char msg[] = "ydaazmziodvwgzhvidajgydnrdyzgtpnzydihvoczhvodxn";
    const char* c = decrypt_caesar(msg);
    printf("%s\n", c);
    free((char *)c);
}