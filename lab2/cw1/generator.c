#include <stdio.h>
#include <ntsid.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <zconf.h>
#include <memory.h>

#define GENERATE_FILE_NAME 2
#define GENERATE_COUNT_OF_ROWS 3
#define GENERATE_BUFFER_SIZE 4
#define GENERATE_IS_SYS 5

void generateBySys(char *name, int size, int sizeOfBuffer);

void generateByLib(char *name, int size, int sizeOfBuffer);

void randomRecord(int sizeOfBuffer, unsigned char *record);

void generate(int argc, char **argv) {
    if (argc < 5) {
        printf("Not enough arguments!!");
        exit(0);
    }

    if (argc == 6 && strcmp(argv[GENERATE_IS_SYS], "lib") == 0) {
        printf("GENERATOR LIB\n");
        generateByLib(argv[GENERATE_FILE_NAME], atoi(argv[GENERATE_COUNT_OF_ROWS]), atoi(argv[GENERATE_BUFFER_SIZE]));
        return;
    }

    printf("GENERATOR SYS\n");
    generateBySys(argv[GENERATE_FILE_NAME], atoi(argv[GENERATE_COUNT_OF_ROWS]), atoi(argv[GENERATE_BUFFER_SIZE]));
}

void generateByLib(char *name, int size, int sizeOfBuffer) {
    unsigned char record[sizeOfBuffer];
    time_t t;
    FILE *handle = fopen(name, "w");

    /* Initializes random number generator */
    srand((unsigned) time(&t));

    for (int i = 0; i < size; ++i) {
        randomRecord(sizeOfBuffer, record);
        fwrite(&record, sizeof(unsigned char), (size_t) sizeOfBuffer, handle);
    }


    fclose(handle);
}

void generateBySys(char *name, int size, int sizeOfBuffer) {
    unsigned char record[sizeOfBuffer];
    time_t t;

    int handle = open(name, O_WRONLY | O_CREAT, 0644);

    /* Initializes random number generator */
    srand((unsigned) time(&t));

    for (int i = 0; i < size; ++i) {
        randomRecord(sizeOfBuffer, record);
        write(handle, &record, (size_t) sizeOfBuffer);
    }


    close(handle);
}

void randomRecord(int sizeOfBuffer, unsigned char *record) {
    for (int j = 0; j < sizeOfBuffer; ++j) {
        record[j] = (unsigned char) rand() % 255 + 0;
    }
}