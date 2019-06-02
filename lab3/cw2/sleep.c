#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <zconf.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argv, char **argc) {
    printf("Sleep program for %ds!\n", atoi(argc[1]));
    sleep(atoi(argc[1]));

    return 0;
}