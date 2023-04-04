#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "utils.h"
#include "process_manager.h"
#include "queue.h"

int main(int argc, char *argv[]) {
    char *filename;
    char *scheduler;
    char *memory_strategy;
    int quantum;

    int arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "f:s:m:q:"))!=-1){
        switch (arg){
            case 'f':
                filename = optarg;
                break;
            case 's':
                scheduler = optarg;
                break;
            case 'm':
                memory_strategy = optarg;
                break;
            case 'q': 
                quantum = atoi(optarg);
                break;
        }
    }

    // printf("%s, %s, %s, %d\n", filename, scheduler, memory_strategy, quantum);

    process_t processes_list[1000];   // change later
    int num_processes = read_file(filename, &processes_list);

    // for (int i=0; i<4; i++){
    //     printf("%d %s %d %d\n", processes_list[i].time_arrived, processes_list[i].process_name, processes_list[i].service_time, processes_list[i].memory_requirement);
    // }

    // Allocate memory to processes and add them to the ready queue
    queue_t *ready_queue = initialize_queue();
    for (int i=0; i<num_processes; i++){
        allocate_memory(processes_list[i], memory_strategy);
        enqueue(ready_queue, processes_list[i]);
    }

    // print_queue(ready_queue);

    // Schedule processes
    if (scheduler=="SJF"){
        shortest_job_first_scheduler(ready_queue);
    } else if (scheduler=="RR"){
        round_robin_scheduler(ready_queue);
    }

    return 0;
}