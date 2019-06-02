#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <zconf.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include "interpreter.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Specify program src");
        return 0;
    }

    FILE *handle = fopen(argv[1], "r");
    if (!handle) {
        printf("Cannot open file");
        return 0;
    }

    interpret(handle);

    fclose(handle);

    return 0;
}

