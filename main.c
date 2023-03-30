#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct process {
    int time_arrived;
    char process_name[8];
    int service_time;
    int memory_requirement;
} process;

int read_file(char *filename, process *processes_list);

int main(int argc, char *argv[]) {
    // Read command line arguments
    if (argc<5){
        printf("Incorrect number of arguments\n");
        exit(1);
    }

    char *filename = argv[1];
    char *scheduler = argv[2];
    char *memory_strategy = argv[3];
    int quantum = atoi(argv[4]);

    process processes_list[1000];   // change later
    read_file(filename, &processes_list);

    // for (int i=0; i<4; i++){
    //     printf("%d %s %d %d\n", processes_list[i].time_arrived, processes_list[i].process_name, processes_list[i].service_time, processes_list[i].memory_requirement);
    // }

    return 0;
}

/* Read processes from file into an array */
int read_file(char *filename, process *processes_list){
    FILE *file_ptr;

    file_ptr = fopen(filename, "r");
    if (file_ptr==NULL){
        printf("File not found\n");
        exit(1);
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int n = 0;
    while ((linelen = getline(&line, &linecap, file_ptr)) > 0){
        int time_arrived = atoi(strtok(line, " "));
        char *process_name = strtok(NULL, " ");
        int service_time = atoi(strtok(NULL, " "));
        int memory_requirement = atoi(strtok(NULL, " "));

        process process_from_file;
        process_from_file.time_arrived = time_arrived;
        strcpy(process_from_file.process_name, process_name);
        process_from_file.service_time = service_time;
        process_from_file.memory_requirement = memory_requirement;

        processes_list[n] = process_from_file;
        n++;
    }

    fclose(file_ptr);

    return 0;
}

