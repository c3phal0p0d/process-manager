#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process_manager.h"
#include "queue.h"

process_t *create_process(int time_arrived, char* process_name, int service_time, int memory_requirement){
    process_t *process = malloc(sizeof(process_t));
    process->time_arrived = time_arrived;
    strcpy(process->process_name, process_name);
    process->service_time = service_time;
    process->run_time = 0;
    process->memory_requirement = memory_requirement;
    process->state = NONE;

    return process;
}

void print_process(process_t *process){
    printf("%d ", process->time_arrived);
    printf("%s ", process->process_name);
    printf("%d ", process->service_time);
    printf("%d\n", process->memory_requirement);
}

process_t *schedule_process(queue_t *ready_queue, char *scheduler){
    process_t* process_to_run = NULL;

    // Schedule process according to the Shortest Job First algorithm
    if (scheduler=="SJF"){
        int shortest_service_time = -1;

        // Iterate through queue to find process with shortest service time
        node_t *node = ready_queue->front;
        for (int i=0; i<ready_queue->size; i++){
            if (shortest_service_time==-1){ 
                shortest_service_time = node->process->service_time;
            }
            else if (node->process->service_time<shortest_service_time){
                node->process->state = RUNNING;
                process_to_run = node->process;
            }
        }
    } 
    // Schedule process according to the Round Robin algorithm
    else if (scheduler=="RR"){
        
    }

    // remove process from ready queue?

    return process_to_run;
}


int allocate_memory(process_t *process){
    return 0;
}