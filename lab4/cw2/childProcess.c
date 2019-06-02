#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <signal.h>
#include "childProcess.h"

int childReceived = 1, isSigUsr2 = 0;

void initBlockMask();

void stop(int signal);

void listenChild(int signal);

void initChildProcess() {
    signal(SIGUSR1, listenChild);
    signal(SIGUSR2, stop);
    initBlockMask();

    while (isSigUsr2 == 0) {
        pause();
    }
}

void listenChild(int signal) {
    printf("CHILD PROCESS: Received signal from main: %d \n", childReceived++);
    kill(getppid(), SIGUSR1);
}

void initBlockMask() {
    sigset_t maskToBlock;
    sigfillset(&maskToBlock);
    sigdelset(&maskToBlock, SIGUSR1);
    sigdelset(&maskToBlock, SIGUSR2);

    sigprocmask(SIG_BLOCK, &maskToBlock, NULL);
}

void stop(int signal) {
    isSigUsr2 = 1;
    printf("Received SIGUSR2. Child process exited... \n");
    exit(0);
}
