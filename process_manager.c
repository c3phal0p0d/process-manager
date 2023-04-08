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
    process->memory_requirement = memory_requirement;
    process->state = NULL;

    return process;
}

void print_process(process_t *process){
    printf("%d ", process->time_arrived);
    printf("%s ", process->process_name);
    printf("%d ", process->service_time);
    printf("%d\n", process->memory_requirement);
}

void shortest_job_first_scheduler(queue_t *ready_queue){
    int shortest_service_time = -1;
    process_t process;

    // iterate through queue
    for (int i; i<ready_queue->size; i++){
        // process = ready_queue process
        if (shortest_service_time==-1){ 
            shortest_service_time = process.service_time;
        }
        else if (process.service_time<shortest_service_time){
            // add process to running state
        }
    }
}

void round_robin_scheduler(queue_t *ready_queue){

}