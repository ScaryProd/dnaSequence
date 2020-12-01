#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
//#include "checker.c"

int main(){
    char string1[100] = "hola";
    char string2[] = "key";
    strcat(string1, string2);

    printf("%s", string1);
    return 0;
}