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

    // Process command-line arguments
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

    // printf("*** Args ***\n");
    // printf("%s, %s, %s, %d\n", filename, scheduler, memory_strategy, quantum);

    // Read processes from file
    queue_t *processes_from_file = initialize_queue();
    read_file(filename, processes_from_file);

    // printf("*** Processes from file ***\n");
    // print_queue(processes_from_file);

    int num_cycles = 0;
    int simulation_time = 0;
    queue_t *input_queue = initialize_queue();
    queue_t *ready_queue = initialize_queue();
    process_t *current_process = NULL;

    // Run process manager until there are no more processes in the input queue and no READY or RUNNING processes.
    while (!(is_empty(input_queue) && is_empty(ready_queue) && current_process==NULL)){
        // Add processes to input queue if their arrival time is less than or equal to the current simulation time
        input_queue = initialize_queue();
        node_t *node = processes_from_file->front;
        while (node!=NULL && node->process->time_arrived<=simulation_time){
            enqueue(input_queue, node->process);
            node = node->next;
        }

        // printf("*** Input queue ***\n");
        // print_queue(input_queue);

        // Allocate memory to processes and add them to the ready queue
        ready_queue = initialize_queue();
        node = input_queue->front;
        for (int i=0; i<input_queue->size; i++){
            if (memory_strategy=="best-fit"){
                allocate_memory(node->process);
                printf("%d,READY,process_name=%s,assigned_at=%d\n", simulation_time, node->process->process_name, simulation_time);
            }
            node->process->state = READY;
            enqueue(ready_queue, node->process);
            node = node->next;
        }

        // printf("*** Ready queue ***\n");
        // print_queue(ready_queue);

        // Schedule process
        current_process = schedule_process(ready_queue, scheduler, current_process);
        current_process->state = RUNNING;

        // Process has finished running
        if (current_process->run_time >= current_process->service_time){
            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, current_process->process_name, input_queue->size + ready_queue->size);
        } 
        else {
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", simulation_time, current_process->process_name, current_process->service_time - current_process->run_time);
        }

        simulation_time += quantum;
        num_cycles++;
        current_process->run_time += quantum;
    }

    return 0;
}