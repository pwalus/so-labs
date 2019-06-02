#include <stdio.h>
#include <time.h>
#include <zconf.h>
#include <signal.h>
#include <stdlib.h>

int stop = 1;

void handleSignalStp(int signal) {
    if (stop == 1) {
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
        stop = 0;

        sigset_t newMask, oldMask;
        sigemptyset(&newMask);
        sigaddset(&newMask, SIGTSTP);

        sigprocmask(SIG_UNBLOCK, &newMask, &oldMask);
        pause();
        sigprocmask(SIG_BLOCK, &oldMask, &newMask);
    } else {
        printf("Wybrano kontynuacje...\n");
        stop = 1;
    }
}

void handleSigInt(int signal) {
    printf("Odebrano sygnał SIGINT\n");
    exit(0);
}

int main() {
    time_t theTime = time(NULL);
    struct tm *timeStructure;

    struct sigaction act;
    act.sa_handler = handleSignalStp;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);


    signal(SIGINT, handleSigInt);

    printf("%d\n", getpid());
    while (1) {
        timeStructure = localtime(&theTime);

        printf("%d:%d:%d\n", timeStructure->tm_hour, timeStructure->tm_min, timeStructure->tm_sec);
        sleep(3);
    }


    return 0;
}