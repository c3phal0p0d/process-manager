#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process_manager.h"
#include "queue.h"

/* Read processes from file into input queue */
int read_file(char *filename, queue_t *input_queue){
    FILE *file_ptr;

    file_ptr = fopen(filename, "r");
    if (file_ptr==NULL){
        printf("File not found\n");
        exit(1);
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int num_processes = 0;
    while ((linelen = getline(&line, &linecap, file_ptr)) > 0){
        int time_arrived = atoi(strtok(line, " "));
        char *process_name = strtok(NULL, " ");
        int service_time = atoi(strtok(NULL, " "));
        int memory_requirement = atoi(strtok(NULL, " "));

        process_t *process = create_process(time_arrived, process_name, service_time, memory_requirement);

        enqueue(input_queue, process);
        num_processes++;
    }

    return num_processes;

    fclose(file_ptr);
}