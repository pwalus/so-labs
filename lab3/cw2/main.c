#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <zconf.h>
#include <sys/wait.h>
#include <stdlib.h>

int runProcess(char *executable, char *arguments);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Specify program src");
        return 0;
    }


    FILE *handle = fopen(argv[1], "r");
    if (!handle) {
        printf("Cannot open file");
        return 0;
    }

    char line[255];
    int i;
    int j_value;
    char executable[20];
    char arguments[20];

    while (fgets(line, 255, handle) != NULL) {
        for (int k = 0; k < 20; ++k) {
            executable[k] = '\0';
            arguments[k] = '\0';
        }

        j_value = 0;
        for (i = 0; i < 255; ++i) {
            if (isspace((int) line[i])) {
                break;
            }

            executable[i] = line[i];
        }

        for (int j = i + 1; j < 255; ++j) {
            if (strcmp(&line[j], "\0") == 0) {
                break;
            }
            if (strcmp(&line[j], "\n") == 0) {
                break;
            }
            arguments[j_value] = line[j];
            j_value++;
        }

        arguments[j_value] = '\0';

        runProcess(executable, arguments);
    }


    return 0;
}

int runProcess(char *executable, char *arguments) {
    pid_t childPid;
    int *status;
    pid_t pid;

    if ((childPid = fork()) < 0) {
        perror("Problem with fork");
        exit(1);
    }


    if (childPid == 0) {
        // CHILD PROCESS
        // -1 == ERROR WITH EXECUTING

        printf("\n\n------RUNNING------: %s %s\n\n", executable, arguments);
        if (execlp(executable, executable, arguments, (char *) 0) == -1) {
            printf("Error in exec: %s --- args: %s \n", executable, arguments);
            exit(-1);
        }

        exit(0);
    } else {
        // MAIN PROCESS

        pid = waitpid(childPid, (int *) &status, 0);

        printf("Process with pid = %d exited with status: %d\n", pid, WEXITSTATUS(status));
        if (WEXITSTATUS(status) != 0) {
            exit(-1);
        }
    }

    return 0;
}