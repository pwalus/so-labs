#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/file.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "global.h"

struct message receivedMessage;

mqd_t initQueue();

sem_t* initBarberSemaphore();

sem_t* initInvitationSemaphore(int numberOfSeats);

sem_t* initSeatSemaphore(int numberOfSeats);

sem_t* initCutSemaphore(int numberOfSeats);

void sendInvitation(sem_t* invitation);

void waitForSit(sem_t* seat);

void sendEnd(sem_t* cut);

void sleepBarber(sem_t* barber);

void printTime();

int *initIsSleeping();

int *initNumberOfSeats(int number);

int *initTakenSeats();

void stopBarber(int signal);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong arguments\n");
        exit(-1);
    }

    int numberOfSeatsCommand = atoi(argv[1]);

    mqd_t serverId = initQueue();
    sem_t *barber = initBarberSemaphore();
    sem_t *invitation = initInvitationSemaphore(numberOfSeatsCommand);
    sem_t *seat = initSeatSemaphore(numberOfSeatsCommand);
    sem_t *cut = initCutSemaphore(numberOfSeatsCommand);

    int *isSleeping = initIsSleeping();
    initNumberOfSeats(numberOfSeatsCommand);
    int *takenSeats = initTakenSeats();

    struct sigaction signalData;
    signalData.sa_flags = 0;
    sigemptyset(&signalData.sa_mask);
    signalData.sa_handler = stopBarber;
    sigaction(SIGINT, &signalData, 0);

    while (1) {
        if (*takenSeats == 0) {
            *isSleeping = BARBER_SLEEP;

            sleepBarber(barber);

            *isSleeping = BARBER_NOT_SLEEP;
            printf("BARBER: woke up\n");
        }

        printf("BARBER: get clients from queue\n");
        if (mq_receive(serverId, (char *)&receivedMessage, MAX_SIZE + 50, NULL) == -1) {
            perror("BARBER: error with msrcv");
            exit(1);
        }

        printf("BARBER: ID SEMAPHORE%d \n", receivedMessage.clientSeatId);

        sendInvitation(invitation);
        waitForSit(seat);

        printf("BARBER: CLIENT is cut %d\n", receivedMessage.clientPid);
        sleep(4);

        sendEnd(cut);
    }
}

void printTime() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    long duration = time.tv_sec + time.tv_nsec;
    printf("%li ----- ", duration);
}

void sendInvitation(sem_t* invitation) {
    printTime();
    printf("BARBER: invites client to chair %d\n", receivedMessage.clientPid);

    sem_post(invitation);

}

void waitForSit(sem_t* seat) {
    sem_wait(seat);

    printTime();
    printf("BARBER: client sit down %d\n", receivedMessage.clientPid);
}

void sendEnd(sem_t* cut) {
    printTime();
    printf("BARBER: ended cutting client %d\n", receivedMessage.clientPid);
    sem_post(cut);
}

void sleepBarber(sem_t* barber) {
   sem_wait(barber);

}

mqd_t initQueue() {
    mqd_t serverId;
    if ((serverId = mq_open(QUEUE_PATH_NAME, O_CREAT | O_RDWR, PERMISSIONS, NULL)) == -1) {
        perror("BARBER: Cannot create queue");
        exit(1);
    }

    return serverId;
}

sem_t* initBarberSemaphore() {
    return sem_open(SEMAPHORE_PATH_NAME, O_CREAT | O_RDWR, PERMISSIONS, 0);
}

sem_t* initInvitationSemaphore(int numberOfSeats) {
    return sem_open(SEMAPHORE_INVITATION_NAME, O_CREAT | O_RDWR, PERMISSIONS, 0);
}

sem_t* initSeatSemaphore(int numberOfSeats) {
    return sem_open(SEMAPHORE_SEAT_NAME, O_CREAT | O_RDWR, PERMISSIONS, 0);
}

sem_t* initCutSemaphore(int numberOfSeats) {
    return sem_open(SEMAPHORE_CUT_NAME, O_CREAT | O_RDWR, PERMISSIONS, 0);
}

int *initIsSleeping(){
    int isSleepingId;
    int *isSleeping;

    isSleepingId = shm_open("/is_sleeping", O_CREAT|O_RDWR, PERMISSIONS);
    ftruncate(isSleepingId, (sizeof(int)));
    isSleeping = mmap(NULL, (sizeof(int)), PROT_READ | PROT_WRITE, MAP_SHARED, isSleepingId, 0);

    *isSleeping = 1;

    return isSleeping;
}

int *initNumberOfSeats(int number) {
    int numberOfSeatsId;
    int *numberOfSeats;

    numberOfSeatsId = shm_open("/number_of_seats", O_CREAT|O_RDWR, PERMISSIONS);
    ftruncate(numberOfSeatsId, (sizeof(int)));
    numberOfSeats = mmap(NULL, (sizeof(int)), PROT_READ | PROT_WRITE, MAP_SHARED, numberOfSeatsId, 0);

    *numberOfSeats = number;

    return numberOfSeats;
}

int *initTakenSeats() {
    int takenSeatsId;
    int *takenSeats;

    takenSeatsId = shm_open("/taken_seats", O_CREAT|O_RDWR, PERMISSIONS);
    ftruncate(takenSeatsId, (sizeof(int)));
    takenSeats = mmap(NULL, (sizeof(int)), PROT_READ | PROT_WRITE, MAP_SHARED, takenSeatsId, 0);
    *takenSeats = 0;

    return takenSeats;
}

void stopBarber(int signal) {
    printf("-------------------------------\n");
    printf("SERVER: ENDING PROCESS...\n");
    printf("-------------------------------\n");

    execl("./clean", "./clean", NULL);

    kill(receivedMessage.clientPid, SIGKILL);
    exit(1);
}
