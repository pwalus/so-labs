#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <memory.h>
#include <zconf.h>

int filterDir(const struct dirent *dirData);

int fileMeetRequirements(const char *path, struct stat *buffer, char *operator, time_t timeToFilter);

void printPermissions(struct stat *buffer);

void printTime(struct stat *buffer, char *modificationTime);

void list(const char *name, char *operator, time_t timeToFilter) {
    DIR *dir;
    struct dirent *dirData;
    struct stat buffer;
    char modificationTime[26];

    char path[1024];
    char *basePath = realpath(name, NULL);

    if (NULL || !(dir = opendir(name))) {
        return;
    }

    while ((dirData = readdir(dir)) != NULL) {
        if (filterDir(dirData)) {
            continue;
        }

        snprintf(path, 1024, "%s/%s", basePath, dirData->d_name);

        if (fileMeetRequirements(path, &buffer, operator, timeToFilter) == 1) {
            printPermissions(&buffer);
            printTime(&buffer, modificationTime);
            printf("%lli --- %s\n", buffer.st_size, path);
        }

        if (dirData->d_type == DT_DIR) {

            pid_t childPid = fork();
            if(childPid != 0) {
                printf("Proces rodzica: Proces rodzica ma pid:%d\n", (int)getpid());
                printf("Proces rodzica: Proces dziecka ma pid:%d\n", (int)childPid);
            } else {
                printf("Proces dziecka: Proces rodzica ma pid:%d\n",(int)getppid());
                printf("Proces dziecka: Proces dziecka ma pid:%d\n",(int)getpid());
                list(path, operator, timeToFilter);
            }
        }
    }
    closedir(dir);
}

void printTime(struct stat *buffer, char *modificationTime) {
    strftime(modificationTime, 26, "%Y-%m-%d %H:%M:%S", localtime(&((*buffer).st_mtime)));
    printf("%s --- ", modificationTime);
}

void printPermissions(struct stat *buffer) {
    printf((S_ISDIR((*buffer).st_mode)) ? "d" : "-");
    printf(((*buffer).st_mode & S_IRUSR) ? "r" : "-");
    printf(((*buffer).st_mode & S_IWUSR) ? "w" : "-");
    printf(((*buffer).st_mode & S_IXUSR) ? "x" : "-");
    printf(((*buffer).st_mode & S_IRGRP) ? "r" : "-");
    printf(((*buffer).st_mode & S_IWGRP) ? "w" : "-");
    printf(((*buffer).st_mode & S_IXGRP) ? "x" : "-");
    printf(((*buffer).st_mode & S_IROTH) ? "r" : "-");
    printf(((*buffer).st_mode & S_IWOTH) ? "w" : "-");
    printf(((*buffer).st_mode & S_IXOTH) ? "x" : "-");
    printf(" --- ");
}

int fileMeetRequirements(const char *path, struct stat *buffer, char *operator, time_t timeToFilter) {
    lstat(path, buffer);
    if (S_ISLNK((*buffer).st_mode)) {
        return -1;
    }

    stat(path, buffer);

    struct tm *tmTime = localtime(&((*buffer).st_mtime));
    time_t firstTime = mktime(tmTime);

    if (strcmp(operator, "=") == 0) {
        if (firstTime != timeToFilter) {
            return -1;
        }
    }

    if (strcmp(operator, ">") == 0) {
        if (firstTime <= timeToFilter) {
            return -1;
        }
    }

    if (strcmp(operator, "<") == 0) {
        if (firstTime >= timeToFilter) {
            return -1;
        }
    }

    return 1;
}

int filterDir(const struct dirent *dirData) {
    return strcmp(dirData->d_name, ".") == 0
           || strcmp(dirData->d_name, "..") == 0;
}


int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Specify folder dir");
        return 0;
    }

    struct tm date;

    if (strptime(argv[3], "%Y-%m-%d %H:%M:%S", &date) == NULL) {
        printf("\nstrptime failed\n");
        return 0;
    }

    time_t timeToFilter = mktime(&date);

    printf("PID glownego programu: %d\n", (int)getpid());

    list(argv[1], argv[2], timeToFilter);

    return 0;
}