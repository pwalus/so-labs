#include <stdio.h>
#include <zconf.h>
#include <signal.h>
#include <stdlib.h>
#include "childProcess.h"
#include "mainProcess.h"

void listenChild(int signal);

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong number of arguments.\n");

        return 0;
    }

    int L = atoi(argv[1]);
    int type = atoi(argv[2]);

    pid_t childPid;

    if ((childPid = fork()) < 0) {
        perror("Problem with fork.\n");
    }


    if (childPid == 0) {
        initChildProcess();
    } else {
        printf("MainPid: %d\n", getpid());
        printf("ChildPid: %d\n", childPid);

        initMainProcess(L, type, childPid);
    }

    return 0;
}