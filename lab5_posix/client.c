#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <time.h>
#include "global.h"
#include "unistd.h"
#include "mqueue.h"

mqd_t serverQ, clientQ;

struct message sendMessage, receivedMessage;
char *clientQueueName;

void sendInitMessage();

void sendTimeMessage();

void sendEndMessage();

void closeQueue();

void stopClient(int signal);

void randomString(size_t length, char *randomString);

int main() {
    srand(time(0));
    clientQueueName = malloc(10);
    randomString(10, clientQueueName);

    if((clientQ = mq_open(clientQueueName, O_CREAT | O_RDWR, 0660, NULL)) == -1){
        perror("CLIENT: Cannot create private queue");
        exit(-1);
    }

    if ((serverQ = mq_open(QUEUE_NAME, O_RDWR)) == -1) {
        perror("CLIENT: Cannot create queue");
        exit(-1);
    }

    struct sigaction signalData;
    signalData.sa_flags = 0;
    sigemptyset(&signalData.sa_mask);
    signalData.sa_handler = stopClient;
    sigaction(SIGINT, &signalData, 0);

    printf("CLIENT: Hello, World!\n");

    sendInitMessage();

    for (int i = 0; i < 10; ++i) {
        sendTimeMessage();
        sleep(1);
    }

    sendEndMessage();

    closeQueue();

    return 0;
}

void sendInitMessage() {
    memcpy(sendMessage.message_content.clientQ, clientQueueName, 10);
    sendMessage.message_content.clientPid = getpid();
    sendMessage.type = INIT;

    printf("---------------\n");
    printf("CLIENT: Client PID: %d\n", sendMessage.message_content.clientPid);
    printf("CLIENT: ClientQ: %s\n", sendMessage.message_content.clientQ);

    if (mq_send(serverQ, (const char *)&sendMessage, sizeof(sendMessage), 0) == -1) {
        perror("CLIENT: Error with mq_send");
        exit(-1);
    }

    if(mq_receive(clientQ, (char *)&receivedMessage, MAX_SIZE + 50, NULL) == -1){
        perror("CLIENT: Error with mq_receive");
        exit(-1);
    }

    printf("CLIENT: Registered. ClientId: %d\n", receivedMessage.message_content.clientId);
}


void sendTimeMessage() {
    sendMessage.type = TIME;

    if(mq_send(serverQ, (char *)&sendMessage, sizeof(sendMessage), 0) == -1){
        perror("CLIENT: Error with mq_send");
        exit(-1);
    }

    if(mq_receive(clientQ, (char *)&receivedMessage, MAX_SIZE + 50, NULL) == -1){
        perror("CLIENT: Error with mq_receive");
        exit(-1);
    }

    printf("CLIENT: ClientId: %d TIME message: %s",
           receivedMessage.message_content.clientId,
           receivedMessage.message_content.content);
}

void sendEndMessage() {
    sendMessage.type = END;

    if(mq_send(serverQ, (char *)&sendMessage, sizeof(sendMessage), 0) == -1){
        perror("CLIENT: Error with mq_send");
        exit(-1);
    }

    printf("CLIENT: Send end message\n");
}

void closeQueue() {
    if (mq_unlink(clientQueueName) == -1) {
        perror("CLIENT: Cannot delete queue");
        exit(-1);
    }
}

void stopClient(int signal) {
    printf("\nCLIENT: Stop request\n");

    sendMessage.type = STOP;

    if(mq_send(serverQ, (char *)&sendMessage, sizeof(sendMessage), 0) == -1){
        perror("CLIENT: Error with mq_send");
        exit(-1);
    }

    exit(1);
}

void randomString(size_t length, char *randomString) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";

    if (length) {
        if (randomString) {
            int l = (int) (sizeof(charset) -1);
            randomString[0] = '/';
            for (int n = 1;n < length;n++) {
                int key = rand() % l;
                randomString[n] = charset[key];
            }

            randomString[length] = '\0';
        }
    }
}
