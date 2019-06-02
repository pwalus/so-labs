#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include "mainProcess.h"


int send = 1, mainReceived = 1;
pid_t pid;

void listenMain(int signal);

void killAll(int signal);

void initMainProcess(int L, int type, pid_t childPid) {
    pid = childPid;

    signal(SIGUSR1, listenMain);
    signal(SIGINT, killAll);

    sleep(2);
    for (int i = 0; i < L; ++i) {
        printf("Send SIGUSR1: %d \n", send++);

        switch (type) {
            case 1:
                kill(childPid, SIGUSR1);
                break;
            case 2:
                kill(childPid, SIGUSR1);
                printf("Waiting for child process...\n");
                pause();
                break;
            case 3:
                // Not available on macOs
//                kill(childPid, SIGRTMIN);
                break;
            default:
                break;
        }


        sleep(1);
    }

    printf("Send SIGUSR2\n");
    kill(childPid, SIGUSR2);
}


void listenMain(int signal) {
    printf("MAIN PROCESS: Received signal from child: %d \n", mainReceived++);
}

void killAll(int signal) {
    printf("Received SIGINT. Exit child and main process... \n");
    kill(pid, SIGUSR2);
    exit(0);
}