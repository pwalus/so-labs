#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include "global.h"
#include <unistd.h>

pthread_mutex_t fullMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fullCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t emptyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t emptyCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;

static volatile int used = 0;

FILE *handle;
char *buffer[LENGTH];
int in = 0, out = 0;

static volatile int end = 0;

void signalHandler(int signal) {
    end = 1;
    kill(getpid(), SIGKILL);
}

void *producer(void *id_ptr) {
    int ID = *((int *) id_ptr);

    char produced[8096];
    while (fscanf(handle, "%s", produced) == 1) {
        if (end == 1) {
            if (FULL_REPORT == 1) {
                printf("PRODUCER: END\n");
            }
            return (void *) 1;
        }

        // FULL CONDITION
        pthread_mutex_lock(&fullMutex);
        while (used == LENGTH) {
            printf("PRODUCER: %d WAIT.. \n", used);
            pthread_cond_wait(&fullCond, &fullMutex);
        }
        pthread_mutex_unlock(&fullMutex);


        pthread_mutex_lock(&bufferMutex);

        buffer[in] = malloc(8096);
        strcpy(buffer[in], produced);
        if (FULL_REPORT == 1) {
            printf("PRODUCER: %d. produced --- %s in %d\n", ID, buffer[in], in);
        }
        in = (in + 1) % LENGTH;
        used++;

        pthread_mutex_unlock(&bufferMutex);

        // EMPTY CONDITION
        pthread_mutex_lock(&emptyMutex);
        pthread_cond_broadcast(&emptyCond);
        pthread_mutex_unlock(&emptyMutex);
    }

    exit(1);
}

void *consumer(void *id_ptr) {
    int ID = *((int *) id_ptr);
    char consumed[8096];

    while (end == 0) {
        // EMPTY CONDITION
        pthread_mutex_lock(&emptyMutex);
        while (used == 0) {
            printf("CONSUMER: WAIT.. \n");
            pthread_cond_wait(&emptyCond, &emptyMutex);
        }
        pthread_mutex_unlock(&emptyMutex);

        pthread_mutex_lock(&bufferMutex);
        strcpy(consumed, buffer[out]);

        if (strcmp(CONDITION, "=") == 0) {
            if (strlen(consumed) == LENGTH_OF_WORD) {
                printf("= CONSUMER: %d consume --- %s from row %d\n", ID, consumed, out);
            }
        }
        if (strcmp(CONDITION, ">") == 0) {
            if (strlen(consumed) > LENGTH_OF_WORD) {
                printf("> CONSUMER: %d consume --- %s from row %d\n", ID, consumed, out);
            }
        }
        if (strcmp(CONDITION, "<") == 0) {
            if (strlen(consumed) < LENGTH_OF_WORD) {
                printf("< CONSUMER: %d consume --- %s from row %d\n", ID, consumed, out);
            }
        }

        buffer[out] = NULL;
        out = (out + 1) % LENGTH;
        used--;

        pthread_mutex_unlock(&bufferMutex);

        // FULL CONDITION
        pthread_mutex_lock(&fullMutex);
        pthread_cond_broadcast(&fullCond);
        pthread_mutex_unlock(&fullMutex);
    }
    printf("CONSUMER: END\n");

    return (void *) 1;
}


int main() {
    handle = fopen(FILE_NAME, "r");
    if (!handle) {
        printf("Cannot open file");
        return 0;
    }

    signal(SIGINT, signalHandler);

    for (int i = 0; i < LENGTH; i++) {
        buffer[i] = NULL;
    }

    pthread_t TID[PRODUCERS + CONSUMERS];
    int id = 0;

    for (int i = 0; i < PRODUCERS; i++) {
        pthread_create(&TID[id], NULL, producer, (void *) &i);
        printf("Producer = %d created\n", i);

        id++;
    }

    for (int i = 0; i < CONSUMERS; i++) {
        pthread_create(&TID[id], NULL, consumer, (void *) &i);
        printf("Consumer = %d created\n", i);

        id++;
    }

    for (int i = 0; i < (PRODUCERS + CONSUMERS); i++) {
        pthread_join(TID[i], NULL);
    }

    return 0;
}