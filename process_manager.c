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
    process->memory_address = -1;
    process->state = NONE;

    return process;
}

void print_process(process_t *process){
    if (process==NULL){
        printf("NULL\n");
        return;
    }
    printf("%d ", process->time_arrived);
    printf("%s ", process->process_name);
    printf("%d ", process->service_time);
    printf("%d\n", process->memory_requirement);
}

process_t *schedule_process(queue_t *ready_queue, char *scheduler, process_t *current_process){
    process_t* process_to_run = NULL;
    // printf("scheduling...\n");

    // Schedule process according to the Shortest Job First algorithm
    if (strcmp(scheduler, "SJF")==0){
        // printf("sjf\n");
        // If current process has not yet finished, it will continue to run
        if (current_process!=NULL && current_process->service_time > current_process->run_time){
            process_to_run = current_process;
            return process_to_run;
        }

        // If current process has finished running
        if (current_process!=NULL && current_process->service_time <= current_process->run_time){
            // printf("finished\n");
            current_process->state = FINISHED;
        }

        int shortest_service_time = -1;

        // Iterate through queue to find process with shortest service time
        if (is_empty(ready_queue)){
            return NULL;
        }
        node_t *node = ready_queue->front;
        for (int i=0; i<ready_queue->size; i++){
            // printf("iterating: \n");
            // print_process(node->process);
            if (shortest_service_time==-1 || node->process->service_time<shortest_service_time){
                // TODO: add checks for breaking ties - shortest arrival time, then lastly lexicographical order
                // printf("found shorter\n");
                process_to_run = node->process;
                shortest_service_time = node->process->service_time;
                // printf("process to remove: ");
                // print_process(node->process);
                // printf("before:\n");
                // print_queue(ready_queue);
                // printf("after:\n");
                // print_queue(ready_queue);
                // printf("removed\n");
            }
            if (node->next==NULL){
                break;
            }
            node = node->next;
        }
        // printf("process to run:\n");
        // print_process(process_to_run);
        process_to_run->state = RUNNING;
        remove_from_queue(ready_queue, process_to_run);
        // printf("shortest service time: %d\n", shortest_service_time);
    } 

    // Schedule process according to the Round Robin algorithm
    else if (strcmp(scheduler, "RR")==0){
        // If current process has finished running
        if (current_process!=NULL && current_process->service_time <= current_process->run_time){
            current_process->state = FINISHED;
        }

        // If current process has not finished running, it should stop running and be placed at end of ready queue
        if (current_process!=NULL && current_process->service_time > current_process->run_time){
            current_process->state = READY;
            enqueue(ready_queue, current_process);
        }

        process_to_run = dequeue(ready_queue);

        if (process_to_run!=NULL){
            process_to_run->state = RUNNING;
        }
    }

    // printf("process: ");
    // print_process(process_to_run);

    return process_to_run;
}

int allocate_process_memory(int *memory, process_t *process){
    //printf("allocating process memory\n");
    // Determine best fit hole in memory
    int hole_size = 0;
    int best_fit_hole_size = -1;
    int memory_address = 0;
    int best_fit_memory_address = -1;
    for (int i=0; i<2048; i++){
        // Memory segment is free
        if (memory[i]==0 && i<2047){
            //printf("hole size: %d\n", hole_size);
            // Start of a new hole in memory
            if (hole_size==0){
                // printf("hole size: 0, i: %d\n", i);
                memory_address = i;
                hole_size = 1;
            }
            // Continuation of a hole in memory
            else if (hole_size>0){
                hole_size++;
            }
        }
        // Memory segment is occupied
        else if (memory[i]==1){
            //printf("i: %d\n", i);
            // Found better fit
            if (hole_size>0 && (best_fit_hole_size==-1 || (hole_size<best_fit_hole_size && hole_size>process->memory_requirement))){
                // printf("found better fit starting at i: %d\n", i);
                best_fit_hole_size = hole_size;
                best_fit_memory_address = memory_address;
            }
            hole_size = 0;
        }
        // Reached end of memory
        else if (i==2047){
            hole_size++;
            if (best_fit_hole_size==-1 || (hole_size<best_fit_hole_size && hole_size>process->memory_requirement)){
                // printf("found better fit\n");
                // printf("memory address: %d\n", memory_address);
                best_fit_hole_size = hole_size;
                best_fit_memory_address = memory_address;
                // printf("best fit memory addr: %d\n", best_fit_memory_address);
            }
            hole_size = 0;
        }
    }

    // Allocate memory when best fit has been found
    if (best_fit_memory_address!=-1){
        for (int i=best_fit_memory_address; i<best_fit_memory_address+process->memory_requirement; i++){
            memory[i] = 1;
        }
        process->memory_address = best_fit_memory_address;
        //printf("process memory address: %d\n", process->memory_address);
        return best_fit_memory_address;
    }

    // Memory could not be allocated to process
    return -1;
}

void free_process_memory(int *memory, process_t *process){
    //printf("freeing process memory\n");
    if (process->memory_address==-1){
        return;
    }
    for (int i=process->memory_address; i<process->memory_address+process->memory_requirement; i++){
        memory[i] = 0;
    }
    //printf("freed\n");
}