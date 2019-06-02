#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zconf.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>
#include <memory.h>
#include <signal.h>

static int display(const char *path, const struct stat *stat, int flag, struct FTW *buffer);

void runTime() {
    struct timespec start, end;
    double time;

    clock_gettime(CLOCK_REALTIME, &start);

    sleep(2);


    clock_gettime(CLOCK_REALTIME, &end);
    time = (double) (end.tv_sec - start.tv_sec) + ((double) (end.tv_nsec - start.tv_nsec) / 1000000000L);

    printf("%f", time);
}


void runAlloc() {
    int *cos = malloc(sizeof(int));
    int *array = calloc(100, sizeof(int));

}


void openSys() {
    int handle = open("../test.txt", O_CREAT | O_RDWR, 0755);
    char *text = calloc(100, sizeof(char));

    read(handle, text, 3);
    char var[50] = "adfbdogsgbskbgfusbgsbsb sbgufs gf";

    lseek(handle, 6, SEEK_SET);
    lseek(handle, 6, SEEK_CUR);
    lseek(handle, 6, SEEK_END);


    write(handle, &var, 23);

    close(handle);
}

void openLib() {
    FILE *handle = fopen("../testowy.txt", "w+");

    char text[50] = "TESTOWY TEKST";
    char *read = calloc(3, sizeof(char));

    fwrite(&text, sizeof(char), 5, handle);


    fseek(handle, 0, 0);
    fread(read, sizeof(char), 3, handle);


    fclose(handle);
}


void goForDir() {
    DIR *dir = opendir("/Users/patrykwalus/CLionProjects");
    struct stat dirData;
    struct dirent *data;

    while ((data = readdir(dir))) {
        if (strcmp(data->d_name, ".") == 0 || strcmp(data->d_name, "..") == 0) {
            continue;
        }

        stat(data->d_name, &dirData);

        printf("%s\n", data->d_name);
    }

//    DIR *dir = opendir("/Users/patrykwalus/CLionProjects");
//    struct dirent *data;
//    struct stat buffer;
//    char path[1024];
//    char *basePath = realpath("/Users/patrykwalus/CLionProjects", NULL);
//
//    while ((data = readdir(dir))) {
//        snprintf(path, 1024, "%s/%s", basePath, data->d_name);
//
//        stat(path, &buffer);
//
//        printf(((buffer).st_mode & S_IRUSR) ? "r" : "-");
//
//
//        printf("%s\n", path);
//    }
//
//
//    closedir(dir);


//    nftw("/Users/patrykwalus/CLionProjects", display, 1000, 0 | FTW_PHYS);

}


static int display(const char *path, const struct stat *stat, int flag, struct FTW *buffer) {
    char *basePath = realpath(path, NULL);
    char newPath[1024];


    printf((S_ISDIR(stat->st_mode)) ? "d" : "-");
    printf(stat->st_mode & S_IRUSR ? "r" : "-");


    printf(" %s\n", realpath(path, NULL));

    return 1;
}

void makeProcess() {
    printf("Parent Process\n");
    pid_t childPid;
    int *status = NULL;

    if ((childPid = fork()) < 0) {
        printf("Problem with fork\n");
    }

    if (childPid == 0) {
        printf("Child Process: Child Pid: %d\n", getpid());
        printf("Child Process: Main Pid: %d \n", getppid());

        execlp("ls", "ls", "-la", NULL);

        printf("NOTHING\n");
        sleep(1);
    } else {
        printf("Main Process: Child Pid: %d \n", childPid);
        printf("Main Process: Main Pid: %d \n", getpid());
        printf("Waiting for child process...\n");
        waitpid(childPid, status, 0);
        printf("Child process eneded...\n");
        printf("Status of exited child: %d", WEXITSTATUS(status));
    }


}

void intProcess(int signal) {
    printf("INT PROCESS\n");


}

void stopProcess(int signal) {
    printf("STOP PROCESS\n");

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);

    sigsuspend(&mask);
}

void makeSignals() {
    signal(SIGINT, intProcess);

    struct sigaction data;
    data.sa_handler = stopProcess;
    sigemptyset(&data.sa_mask);
    data.sa_flags = 0;


    sigaction(SIGTSTP, &data, NULL);

    while (1) {
        printf("SLEEP...\n");
        sleep(1);
    }
}

void makePipe() {
    pid_t childPid;

    int fd[2];
    pipe(fd);

    char text[50];

    scanf("%s", text);

    if ((childPid = fork()) < 0) {
        exit(-1);
    }

    if (childPid == 0) {
        close(fd[1]);
        char readedText[50];

        read(fd[0], readedText, 50);
        close(fd[0]);


        printf("READ: %s", readedText);
        exit(0);
    } else {
        close(fd[0]);

        printf("WRITE: %s", text);
        write(fd[1], text, 50);

        close(fd[1]);

        wait(NULL);
    }


}

void makeDup() {
    pid_t childPid;

    int fd[2];
    pipe(fd);

    char text[50];

    scanf("%s", text);

    if ((childPid = fork()) < 0) {
        exit(-1);
    }

    if (childPid == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execlp("wc", "wc", "-m", NULL);
        close(fd[0]);
    } else {
        close(fd[0]);
        write(fd[1], text, strlen(text) + 1);
        close(fd[1]);
    }

}

int main() {
//    runTime();
//    runAlloc();
//    openSys();
//    openLib();
//    goForDir();
//    makeProcess();
//    makeSignals();
//    makePipe();
//    makeDup();

    FILE *grep_input = popen("grep Ala", "w");
    fputs("fdsfsAlaadfa", grep_input);
    pclose(grep_input);

    return 0;
}
