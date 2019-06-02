//
// Created by Patryk Waluś on 13/11/2018.
//


#include "interpreter.h"
#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <zconf.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>

const int MAX_LENGTH = 255;

const int LENGTH_OF_EXECUTABLE = 5;

struct pipeExec {
    char executable[MAX_LENGTH];
    char arguments[MAX_LENGTH];
};

int inc;

int create(const char *line, struct pipeExec **arrayOfExecutable, int execNumber);

int runProcess(struct pipeExec **arrayOfExecutable);

void interpret(FILE *handle) {
    char line[MAX_LENGTH];

    while (fgets(line, MAX_LENGTH, handle) != NULL) {
        inc = 0;
        struct pipeExec *arrayOfExecutable[LENGTH_OF_EXECUTABLE];
        for (int i = 0; i < LENGTH_OF_EXECUTABLE; ++i) {
            arrayOfExecutable[i] = NULL;
        }

        int execNumber = -1;

        int x;
        do {
            execNumber++;
            arrayOfExecutable[execNumber] = malloc(sizeof(struct pipeExec));
            x = create(line, arrayOfExecutable, execNumber);
        } while (x == 1);

        runProcess(arrayOfExecutable);
        printf("--------------\n");
    }
};


int create(const char *line, struct pipeExec **arrayOfExecutable, int execNumber) {
    int stringInc = 0;
    for (int k = 0; k < MAX_LENGTH; ++k) {
        arrayOfExecutable[execNumber]->executable[k] = '\0';
        arrayOfExecutable[execNumber]->arguments[k] = '\0';
    }

    for (; inc < MAX_LENGTH; ++inc) {
        if (isspace((int) line[inc])) {
            break;
        }

        arrayOfExecutable[execNumber]->executable[stringInc++] = line[inc];
    }

    stringInc = 0;
    for (++inc; inc < MAX_LENGTH; ++inc) {
        if (line[inc] == '|') {
            if (arrayOfExecutable[execNumber]->arguments[stringInc - 1] == ' ') {
                arrayOfExecutable[execNumber]->arguments[stringInc - 1] = '\0';
            }
            inc++;
            inc++;
            return 1;
        }

        if (strcmp(&line[inc], "\0") == 0) {
            break;
        }
        if (strcmp(&line[inc], "\n") == 0) {
            break;
        }

        arrayOfExecutable[execNumber]->arguments[stringInc++] = line[inc];
    }

    arrayOfExecutable[execNumber]->arguments[stringInc] = '\0';

    return 0;
}


int runProcess(struct pipeExec **arrayOfExecutable) {
    int fd1[2];
    int fd2[2];

    // 0 - read
    // 1 - write

    int result, childStatus;
    pid_t childPid;
    int __WRITE__ = 1;
    int __READ__ = 0;

    for (int i = 0; i < LENGTH_OF_EXECUTABLE; ++i) {
        if (i % 2 == 0) {
            pipe(fd1);
        } else {
            pipe(fd2);
        }

        if ((childPid = fork()) < 0) {
            printf("Problem with fork\n");
        }


        if (i == LENGTH_OF_EXECUTABLE - 1) {
            close(fd1[__WRITE__]);
            close(fd2[__WRITE__]);
        }

        if (childPid == 0) {
            if (i == 0) {
                close(fd1[__READ__]);
                dup2(fd1[__WRITE__], STDOUT_FILENO);
            } else if (i % 2 == 0) {
                dup2(fd2[__READ__], STDIN_FILENO);
                close(fd2[__WRITE__]);

                dup2(fd1[__WRITE__], STDOUT_FILENO);
                close(fd1[__READ__]);
            } else {
                dup2(fd1[__READ__], STDIN_FILENO);
                close(fd1[__WRITE__]);

                dup2(fd2[__WRITE__], STDOUT_FILENO);
                close(fd2[__READ__]);
            }

            if (i == LENGTH_OF_EXECUTABLE - 1) {
                printf("\n");
            }
            result = execlp(arrayOfExecutable[i]->executable,
                            arrayOfExecutable[i]->executable,
                            arrayOfExecutable[i]->arguments,
                            (char *) 0);

            if (i != 0 && i % 2 == 0) {
                close(fd2[__READ__]);
            } else {
                close(fd1[__READ__]);
            }


            if (result == -1) {
                exit(-1);
            }

            exit(0);
        } else {
            if (i % 2 == 0) {
                close(fd2[__READ__]);
                close(fd2[__WRITE__]);
            } else {
                close(fd1[__READ__]);
                close(fd1[__WRITE__]);
            }

            printf("Exec now: %s %s\n", arrayOfExecutable[i]->executable, arrayOfExecutable[i]->arguments);
            waitpid(childPid, &childStatus, 0);


            if (WEXITSTATUS(childStatus) != 0) {
                printf("Error with above command...\n");
                exit(-1);
            }
        }
    }

    return 0;
}
