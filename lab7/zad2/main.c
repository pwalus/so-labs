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

sem_t *place, *full, *mutex;

FILE *handle;
char *buffer[LENGTH];
int in = 0, out = 0;

static volatile int end = 0;

void clean();

void signalHandler(int signal) {
    end = 1;
    kill(getpid(), SIGKILL);
}

void *producer(void *id_ptr) {
    int ID = *((int *) id_ptr);

    char produced[1024];
    while (fscanf(handle, "%s", produced) == 1) {
        if (end == 1) {

            if (FULL_REPORT == 1) {
                printf("PRODUCER: END\n");
            }
            return (void *) 1;
        }

        (void) sem_wait(place);
        (void) sem_wait(mutex);

        buffer[in] = malloc(1024);
        strcpy(buffer[in], produced);
        if (FULL_REPORT == 1) {
            printf("PRODUCER: %d. produced --- %s in %d\n", ID, buffer[in], in);
        }
        in = (in + 1) % LENGTH;

        (void) sem_post(full);
        (void) sem_post(mutex);
    }

    exit(1);
}

void *consumer(void *id_ptr) {
    int ID = *((int *) id_ptr);
    char consumed[1024];

    while (end == 0) {
        (void) sem_wait(full);
        (void) sem_wait(mutex);

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

        (void) sem_post(mutex);
        (void) sem_post(place);
    }
    printf("CONSUMER: END\n");

    return (void *) 1;
}


int main() {
    clean();
    handle = fopen(FILE_NAME, "r");
    if (!handle) {
        printf("Cannot open file");
        return 0;
    }

    signal(SIGINT, signalHandler);


    place = sem_open("/place", O_CREAT, 0644, LENGTH);
    full = sem_open("/full", O_CREAT, 0644, 0);
    mutex = sem_open("/mutex", O_CREAT, 0644, 1);

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

    (void) sem_unlink("/place");
    (void) sem_unlink("/full");
    (void) sem_unlink("/mutex");

    return 0;
}

void clean() {
    (void) sem_unlink("/place");
    (void) sem_unlink("/full");
    (void) sem_unlink("/mutex");
}