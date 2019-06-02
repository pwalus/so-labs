#include <stdio.h>
#include <dirent.h>
#include <memory.h>
#include <sys/stat.h>
#include <time.h>
#include <ftw.h>
#include <stdlib.h>

time_t timeToFilter;

char *operator;

int fileMeetRequirements(const struct stat *buffer);

void printPermissions(const struct stat *buffer);

void printTime(const struct stat *buffer, char *modificationTime);

static int display_info(const char *path, const struct stat *stat, int flag, struct FTW *buffer) {
    /* FTW_SL - Symbolic link.  */
    if (flag == FTW_SL) {
        return -1;
    }

    if(fileMeetRequirements(stat) == -1){
        return -1;
    }

    char modificationTime[26];

    printPermissions(stat);
    printTime(stat, modificationTime);
    printf("%s\n", realpath(path, NULL));
    return 0;
}

void printPermissions(const struct stat *buffer) {
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

void printTime(const struct stat *buffer, char *modificationTime) {
    strftime(modificationTime, 26, "%Y-%m-%d %H:%M:%S", localtime(&((*buffer).st_mtime)));
    printf("%s --- ", modificationTime);
}

int fileMeetRequirements(const struct stat *buffer) {
    struct tm *tmTime = localtime(&((*buffer).st_mtime));
    time_t firstTime = mktime(tmTime);

    if(strcmp(operator, "=") == 0){
        if(firstTime != timeToFilter){
            return -1;
        }
    }

    if(strcmp(operator, ">") == 0){
        if(firstTime <= timeToFilter){
            return -1;
        }
    }

    if(strcmp(operator, "<") == 0){
        if(firstTime >= timeToFilter){
            return -1;
        }
    }

    return 1;
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

    timeToFilter = mktime(&date);
    operator = argv[2];

    /* FTW_PHYS - Physical walk, don't follow sym links.  */
    nftw(argv[1], display_info, 1000, 0 | FTW_PHYS);

    return 0;
}