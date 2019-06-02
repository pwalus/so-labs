#include <stdio.h>
#include <memory.h>
#include "generator.h"
#include "copy.h"
#include "sort.h"

#define METHOD 1


int main(int argc, char **argv) {
    if (strcmp(argv[METHOD], "generate") == 0) {
        generate(argc, argv);
    } else if (strcmp(argv[METHOD], "sort") == 0) {
        sort(argc, argv);
    } else if (strcmp(argv[METHOD], "copy") == 0) {
        copy(argc, argv);
    } else {
        printf("Not defined method");
    }


    return 0;
}