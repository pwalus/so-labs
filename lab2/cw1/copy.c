#include <stdio.h>
#include <ntsid.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>

#define COPY_FILE_NAME_FROM 2
#define COPY_FILE_NAME_TO 3
#define COPY_COUNT_OF_ROWS 4
#define COPY_BUFFER_SIZE 5
#define COPY_IS_SYS 6

void copyLib(char *fileNameFrom, char *fileNameTo, int size, int sizeOfBuffer);

void copySys(char *fileNameFrom, char *fileNameTo, int size, int sizeOfBuffer);

void copy(int argc, char **argv) {
    if (argc < 6) {
        printf("Not enough arguments!!");
        exit(0);
    }

    if (argc == 7 && strcmp(argv[COPY_IS_SYS], "lib") == 0) {
        printf("COPY LIB\n");
        copyLib(
                argv[COPY_FILE_NAME_FROM],
                argv[COPY_FILE_NAME_TO],
                atoi(argv[COPY_COUNT_OF_ROWS]),
                atoi(argv[COPY_BUFFER_SIZE])
        );
        return;
    }

    printf("COPY SYS\n");
    copySys(
            argv[COPY_FILE_NAME_FROM],
            argv[COPY_FILE_NAME_TO],
            atoi(argv[COPY_COUNT_OF_ROWS]),
            atoi(argv[COPY_BUFFER_SIZE])
    );
}

void copyLib(char *fileNameFrom, char *fileNameTo, int size, int sizeOfBuffer) {
    FILE *handleFrom = fopen(fileNameFrom, "r");
    if (!handleFrom) {
        printf("FILE NOT EXISTS");
        return;
    }

    FILE *handleTo = fopen(fileNameTo, "w");
    unsigned char record[sizeOfBuffer];
    ssize_t readSize;

    for (int i = 0; i < size; ++i) {
        readSize = fread(&record, sizeof(unsigned char), (size_t) sizeOfBuffer, handleFrom);
        if (readSize <= 0) {
            break;
        }

        fwrite(&record, sizeof(unsigned char), (size_t) sizeOfBuffer, handleTo);
    }

    fclose(handleFrom);
    fclose(handleTo);
}

void copySys(char *fileNameFrom, char *fileNameTo, int size, int sizeOfBuffer) {
    int handleFrom = open(fileNameFrom, O_RDONLY);
    if (handleFrom == -1) {
        printf("FILE NOT EXISTS");
        return;
    }

    int handleTo = open(fileNameTo, O_WRONLY | O_CREAT, 0644);
    unsigned char record[sizeOfBuffer];
    ssize_t readSize;

    for (int i = 0; i < size; ++i) {
        readSize = read(handleFrom, &record, (size_t) sizeOfBuffer);
        if (readSize <= 0) {
            break;
        }

        write(handleTo, &record, (size_t) sizeOfBuffer);
    }

    close(handleFrom);
    close(handleTo);
}