#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/file.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "global.h"

struct message sendMessage;

mqd_t initQueue();

sem_t* initBarberSemaphore();

sem_t* initInvitationSemaphore(int numberOfSeats);

sem_t* initSeatSemaphore(int numberOfSeats);

sem_t* initCutSemaphore(int numberOfSeats);

void waitForInvitation(sem_t* invitation, int seatId);

void sittingOnChair(sem_t* seat, int seatId);

void waitForEndCutting(sem_t* cut, int seatId);

void printTime();

int *initIsSleeping();

int *initNumberOfSeats();

int *initTakenSeats();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Wrong arguments\n");
        exit(-1);
    }

    mqd_t serverId = initQueue();
    sem_t *barber = initBarberSemaphore();
    sem_t *invitation = initInvitationSemaphore(12);
    sem_t *seat = initSeatSemaphore(12);
    sem_t *cut = initCutSemaphore(12);

    int *isSleeping = initIsSleeping();
    int *numberOfSeats = initNumberOfSeats();
    int *takenSeats = initTakenSeats();

    int numberOfCuts = atoi(argv[1]);


            for (int i = 0; i < numberOfCuts; ++i) {
                int seatId = 0;

                if (*isSleeping == BARBER_SLEEP && (*takenSeats == 0)) {
                    sendMessage.clientPid = getpid();

                    if (mq_send(serverId, (const char *)&sendMessage, sizeof(struct message), 0) == -1) {
                        perror("CLIENT: Error with msgsnd");
                        exit(-1);
                    }

                    sem_post(barber);

                    printf("CLIENT %d: Waking up barber\n", getpid());
                } else if (*takenSeats < *numberOfSeats) {
                    (*takenSeats)++;
                    seatId = (*takenSeats);

                    printf("CLIENT %d: Waiting in waiting room...\n", getpid());
                    printf("CLIENT %d: Taken seats: %d\n", getpid(), (*takenSeats));

                    sendMessage.clientPid = getpid();
                    sendMessage.clientSeatId = seatId;

                    if (mq_send(serverId, (const char *)&sendMessage, sizeof(struct message), 0) == -1) {
                        perror("CLIENT: Error with msgsnd");
                        exit(-1);
                    }
                } else {
                    printf("CLIENT %d: No more seats...\n", getpid());
                    sleep(5);
                    i--;

                    continue;
                }

                printf("CLIENT: %d ID SEMAPHORE %d\n", getpid(), seatId);

                waitForInvitation(invitation, seatId);
                sittingOnChair(seat, seatId);
                waitForEndCutting(cut, seatId);

                printTime();
                printf("CLIENT %d: Leaving barber\n", getpid());


                (*takenSeats)--;
                if ((*takenSeats) < 0) {
                    (*takenSeats) = 0;
                }
            }

            printf("CLIENT %d: End of all cuts\n", getpid());



    shmdt(isSleeping);
    shmdt(takenSeats);
    shmdt(numberOfSeats);

    return 0;
}

int *initIsSleeping() {
    int isSleepingId;
    int *isSleeping;

    isSleepingId = shm_open("/is_sleeping", O_RDWR, PERMISSIONS);
    isSleeping = mmap(NULL, (sizeof(int)), PROT_READ | PROT_WRITE, MAP_SHARED, isSleepingId, 0);

    return isSleeping;
}

int *initNumberOfSeats() {
    int numberOfSeatsId;
    int *numberOfSeats;

    numberOfSeatsId = shm_open("/number_of_seats", O_RDWR, PERMISSIONS);
    numberOfSeats = mmap(NULL, (sizeof(int)), PROT_READ | PROT_WRITE, MAP_SHARED, numberOfSeatsId, 0);

    return numberOfSeats;
}

int *initTakenSeats() {
    int takenSeatsId;
    int *takenSeats;

    takenSeatsId = shm_open("/taken_seats", O_RDWR, PERMISSIONS);
    takenSeats = mmap(NULL, (sizeof(int)), PROT_READ | PROT_WRITE, MAP_SHARED, takenSeatsId, 0);

    return takenSeats;
}

void printTime() {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    long duration = time.tv_sec + time.tv_nsec;
    printf("%li ----- ", duration);
}

void waitForInvitation(sem_t* invitation, int seatId) {
    printTime();
    printf("CLIENT %d: Waiting for invitation\n", getpid());

    sem_wait(invitation);
}

void sittingOnChair(sem_t* seat, int seatId) {
    printTime();
    printf("CLIENT %d: Sitting on chair\n", getpid());
    sleep(2);

    sem_post(seat);

}

void waitForEndCutting(sem_t* cut, int seatId) {
    printTime();
    printf("CLIENT %d: Sits on chair and is being cut\n", getpid());

    sem_wait(cut);
}

mqd_t initQueue() {
    mqd_t serverId;

    if ((serverId = mq_open(QUEUE_PATH_NAME, O_RDWR)) == -1) {
        perror("CLIENT: Cannot create queue");
        exit(1);
    }

    return serverId;

}

sem_t* initBarberSemaphore() {
    return sem_open(SEMAPHORE_PATH_NAME, O_RDWR);
}

sem_t* initInvitationSemaphore(int numberOfSeats) {
    return sem_open(SEMAPHORE_INVITATION_NAME,  O_RDWR);
}

sem_t* initSeatSemaphore(int numberOfSeats) {
    return sem_open(SEMAPHORE_SEAT_NAME, O_RDWR);
}

sem_t* initCutSemaphore(int numberOfSeats) {
    return sem_open(SEMAPHORE_CUT_NAME, O_RDWR);
}