#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char* filename;
    char* scheduler;
    char* memory_strategy;
    int quantum;

    // Process command line arguments
    if (argc<5){
        printf("Incorrect number of arguments\n");
        exit(1);
    }

    filename = argv[1];
    scheduler = argv[2];
    memory_strategy = argv[3];
    quantum = atoi(argv[4]);

    return 0;
}
