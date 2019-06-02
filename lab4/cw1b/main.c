#include <stdio.h>
#include <signal.h>
#include <zconf.h>
#include <stdlib.h>

pid_t childPid;

void loop();

void startMain();

void handleSignalInt(int signal) {
    printf("Sygnał SIGINT: %d. Kończę proces potomny oraz macierzysty...\n", signal);
    kill(childPid, SIGKILL);
    exit(0);
}

void handleSignalStp(int signal) {
    if (kill(childPid, 0) == -1) {
        printf("Proces potomny o pid %d już nie istnieje. Startuje na nowo...\n", childPid);

        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGTSTP);

        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        startMain();
        sigprocmask(SIG_BLOCK, &mask, NULL);
    } else {
        printf("Sygnał SIGTSTP: %d. Kończę proces potomny o pid %d...\n", signal, childPid);
        kill(childPid, SIGKILL);
    }
}

void loop() {
    if ((childPid = fork()) < 0) {
        perror("Cannot start new process!");
        exit(0);
    }

    if (childPid == 0) {
        if (execlp("./dateShell", "./dateShell", NULL) == -1) {
            perror("Error when exec date");
        }
    } else {
        printf("main: %d\n", getpid());
        printf("childPid: %d\n", childPid);
        waitpid(childPid, NULL, 0);
    }
}

void startMain() {
    do {
        loop();
        printf("Child process ended. Wait 5s...\n");
        sleep(5);
    } while (kill(childPid, 0) != -1);
}

int main() {
    signal(SIGINT, handleSignalInt);
    signal(SIGTSTP, handleSignalStp);

    startMain();

    return 0;
}
