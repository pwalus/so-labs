//
// Created by Patryk Waluś on 21/11/2018.
//

#ifndef LAB5_GLOBAL_H
#define LAB5_GLOBAL_H

#include <sys/types.h>

#define QUEUE_PATH_NAME "/queue_key"
#define MAX_SIZE 8192

#define SEMAPHORE_PATH_NAME "/semaphore_key"
#define SEMAPHORE_INVITATION_NAME "/invitation_key"
#define SEMAPHORE_SEAT_NAME "/seat_key"
#define SEMAPHORE_CUT_NAME "/cut_key"

#define SEMAPHORE_ID 0

#define PERMISSIONS 0660

#define BARBER_SLEEP 1
#define BARBER_NOT_SLEEP -1


struct message {
    pid_t clientPid;
    int clientSeatId;
};

#endif //LAB5_GLOBAL_H
