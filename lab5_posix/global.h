//
// Created by Patryk Waluś on 21/11/2018.
//

#ifndef LAB5_GLOBAL_H
#define LAB5_GLOBAL_H

#include <unistd.h>
#include <mqueue.h>

#define QUEUE_NAME "/test_queue"
#define INIT 1
#define TIME 2
#define END 3
#define STOP 4
#define MAX_SIZE 8192
#define MAX_CLIENT_NUMBER 10


struct clientData {
    int clientId;
    char clientQueueName[10];
    mqd_t clientQ;
    pid_t clientPid;
};

struct message_content {
    char clientQ[10];
    pid_t clientPid;
    int clientId;
    char content[200];
};

struct message {
    int type;
    struct message_content message_content;
};

#endif //LAB5_GLOBAL_H
