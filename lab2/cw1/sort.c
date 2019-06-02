#include <stdio.h>
#include <ntsid.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>

#define SORT_FILE_NAME 2
#define SORT_COUNT_OF_ROWS 3
#define SORT_BUFFER_SIZE 4
#define SORT_IS_SYS 5

void sortLib(char *fileName, int size, int sizeOfBuffer);

void sortSys(char *fileName, int size, int sizeOfBuffer);

void sort(int argc, char **argv) {
    if (argc < 5) {
        printf("Not enough arguments!!");
        exit(0);
    }

    if (argc == 6 && strcmp(argv[SORT_IS_SYS], "lib") == 0) {
        printf("SORT LIB\n");
        sortLib(
                argv[SORT_FILE_NAME],
                atoi(argv[SORT_COUNT_OF_ROWS]),
                atoi(argv[SORT_BUFFER_SIZE])
        );
        return;
    }

    printf("SORT SYS\n");
    sortSys(
            argv[SORT_FILE_NAME],
            atoi(argv[SORT_COUNT_OF_ROWS]),
            atoi(argv[SORT_BUFFER_SIZE])
    );
}

void sortLib(char *fileName, int size, int sizeOfBuffer) {
    FILE *handle = fopen(fileName, "r+");
    if (!handle) {
        printf("FILE NOT EXISTS");
        return;
    }

    size_t sizeT = (size_t) sizeOfBuffer;
    unsigned char *first = calloc(sizeT, sizeof(unsigned char));
    unsigned char *second = calloc(sizeT, sizeof(unsigned char));
    int posFirst;
    int posSecond;
    for (int i = 1; i < size; ++i) {
        posFirst = i;
        fseek(handle, posFirst * sizeOfBuffer, 0);
        fread(first, sizeof(unsigned char), sizeT, handle);

        for (posSecond = posFirst - 1; posSecond >= 0; posSecond--) {
            fseek(handle, posSecond * sizeOfBuffer, 0);
            fread(second, sizeof(unsigned char), sizeT, handle);

            if (*first < *second) {
                fseek(handle, posSecond * sizeOfBuffer, 0);
                fwrite(first, sizeof(unsigned char), sizeT, handle);

                fseek(handle, posFirst * sizeOfBuffer, 0);
                fwrite(second, sizeof(unsigned char), sizeT, handle);

                posFirst--;
            } else {
                break;
            }
        }
    }

    fclose(handle);
}

void sortSys(char *fileName, int size, int sizeOfBuffer) {
    int handle = open(fileName, O_RDWR);
    if (handle == -1) {
        printf("FILE NOT EXISTS");
        return;
    }

    size_t sizeT = (size_t) sizeOfBuffer;
    unsigned char *first = calloc(sizeT, sizeof(unsigned char));
    unsigned char *second = calloc(sizeT, sizeof(unsigned char));
    int posFirst;
    int posSecond;

    for (int i = 1; i < size; ++i) {
        posFirst = i;
        lseek(handle, posFirst * sizeOfBuffer, SEEK_SET);
        read(handle, first, sizeT);

        for (posSecond = posFirst - 1; posSecond >= 0; posSecond--) {
            lseek(handle, posSecond * sizeOfBuffer, SEEK_SET);
            read(handle, second, sizeT);

            if (*first < *second) {
                lseek(handle, posSecond * sizeOfBuffer, SEEK_SET);
                write(handle, first, sizeT);

                lseek(handle, posFirst * sizeOfBuffer, SEEK_SET);
                write(handle, second, sizeT);

                posFirst--;
            } else {
                break;
            }
        }
    }

    close(handle);
}