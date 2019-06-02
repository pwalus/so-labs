#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/msg.h>

#include <memory.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <time.h>
#include <mqueue.h>
#include "global.h"

#include <string.h>
#include <sys/stat.h>


mqd_t serverQ, clientQ;

struct message receivedMessage, sendMessage;
int registeredNumber = 0;
struct clientData *clientDataArray;

int getClient(struct message *message, const struct clientData *clientDataArray, struct clientData *client);

void registerNewClient(struct clientData *clientDataArray);

void serveTimeMessage(struct message *receivedMessage, struct clientData *client);

void serveEndMessage(struct message *receivedMessage, struct clientData *client);

void serveStopMessage(struct message *receivedMessage, struct clientData *client);

void stopServer(int signal);

int main() {
    clientDataArray = calloc(MAX_CLIENT_NUMBER, sizeof(struct clientData));
    struct clientData *client = malloc(sizeof(struct clientData));

    if((serverQ = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0660, NULL)) == -1){
        perror("SERVER: Cannot create queue");
        exit(-1);
    }

    struct sigaction signalData;
    signalData.sa_flags = 0;
    sigemptyset(&signalData.sa_mask);
    signalData.sa_handler = stopServer;
    sigaction(SIGINT, &signalData, 0);

    printf("SERVER: Hello, World!\n");

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);


    while (1) {
        sigprocmask(SIG_BLOCK, &mask, NULL);
        if(mq_receive(serverQ, (char *)&receivedMessage, MAX_SIZE + 50, NULL) == -1){
            perror("SERVER: error with mq_receive");
            exit(1);
        }
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        printf("SERVER: message received.\n");

        if (getClient(&receivedMessage, clientDataArray, client) == -1 && receivedMessage.type != INIT) {
            printf("SERVER: Client have to first send INIT message, to register!\n");
            continue;
        }

        if (receivedMessage.type == INIT) {
            registerNewClient(clientDataArray);
        } else if (receivedMessage.type == TIME) {
            serveTimeMessage(&receivedMessage, client);
        } else if (receivedMessage.type == END) {
            serveEndMessage(&receivedMessage, client);
        } else if (receivedMessage.type == STOP) {
            serveStopMessage(&receivedMessage, client);
        }

    }

    return 0;
}

void registerNewClient(struct clientData *clientDataArray) {
    if (registeredNumber == MAX_CLIENT_NUMBER) {
        perror("Cannot register new client\n");
    }

    if ((clientQ = mq_open(receivedMessage.message_content.clientQ, O_RDWR)) == -1) {
        perror("SERVER: Cannot create queue");
        exit(-1);
    }


    clientDataArray[registeredNumber].clientId = registeredNumber;
    clientDataArray[registeredNumber].clientPid = receivedMessage.message_content.clientPid;
    clientDataArray[registeredNumber].clientQ = clientQ;
    memcpy(clientDataArray[registeredNumber].clientQueueName, receivedMessage.message_content.clientQ, 10);

    sendMessage.message_content.clientPid = receivedMessage.message_content.clientPid;
    sendMessage.message_content.clientId = registeredNumber;
    sendMessage.type = INIT;

    registeredNumber++;

    printf("-------REGISTERING--------\n");
    printf("SERVER: Client PID: %d\n", sendMessage.message_content.clientPid);
    printf("SERVER: Client ID: %d\n", sendMessage.message_content.clientId);
    printf("SERVER: ClientQ: %s\n", receivedMessage.message_content.clientQ);
    printf("---------------------------\n");

    if(mq_send(clientQ, (const char*)&sendMessage, sizeof(sendMessage), 0) == -1){
        perror("SERVER: Cannot register new client");
        exit(-1);
    }

}

void serveTimeMessage(struct message *receivedMessage, struct clientData *client) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    snprintf(sendMessage.message_content.content, 200, "%d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,
             tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    sendMessage.type = TIME;
    sendMessage.message_content.clientPid = client->clientPid;
    sendMessage.message_content.clientId = client->clientId;

    if(mq_send(clientQ, (const char*)&sendMessage, sizeof(sendMessage), 0) == -1){
        perror("SERVER: Cannot send time message");
        exit(-1);
    }
}

void serveEndMessage(struct message *receivedMessage, struct clientData *client) {
    if(mq_unlink(QUEUE_NAME) == -1){
        perror("SERVER: Cannot close queue");
    }

    printf("SERVER: Stop receiving messages\n");
}

void serveStopMessage(struct message *receivedMessage, struct clientData *client) {
    printf("-------------------------------\n");
    printf("SERVER: Killing client queue\n");
    printf("-------------------------------\n");

    if (mq_unlink(client->clientQueueName) == -1) {
        perror("SERVER: Cannot kill queue");
    }
}


int getClient(struct message *message, const struct clientData *clientDataArray, struct clientData *client) {
    for (int i = 0; i < MAX_CLIENT_NUMBER; ++i) {
        if (clientDataArray[i].clientPid == (*message).message_content.clientPid) {
            printf("SERVER: CLIENT ALREADY REGISTER. CLIENT_ID: %d\n", clientDataArray[i].clientId);
            client->clientId = clientDataArray[i].clientId;
            client->clientPid = clientDataArray[i].clientPid;
            client->clientQ = clientDataArray[i].clientQ;
            memcpy(client->clientQueueName, clientDataArray[i].clientQueueName, 10);

            return 1;
        }
    }

    return -1;
}

void stopServer(int signal) {
    printf("-------------------------------\n");
    printf("SERVER: Killing clients...\n");
    printf("-------------------------------\n");
    for (int i = 0; i < MAX_CLIENT_NUMBER; ++i) {
        if (clientDataArray[i].clientPid) {
            printf("SERVER: Kill: %d\n", clientDataArray[i].clientPid);
            kill(clientDataArray[i].clientPid, SIGKILL);
            mq_unlink(clientDataArray[i].clientQueueName);
        }
    }

    mq_unlink(QUEUE_NAME);
    exit(1);
}
