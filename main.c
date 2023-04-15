#define IMPLEMENTS_REAL_PROCESS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "utils.h"
#include "process_manager.h"
#include "queue.h"

int main(int argc, char *argv[]) {
    char *filename = NULL;
    char *scheduler = NULL;
    char *memory_strategy = NULL;
    int quantum = -1;

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

    // Read processes from file
    queue_t *processes_from_file = initialize_queue();
    read_file(filename, processes_from_file);
    int num_proc_from_file_left = processes_from_file->size;

    int memory[2048] = {0};

    queue_t *input_queue = initialize_queue();
    queue_t *ready_queue = initialize_queue();
    process_t *current_process = NULL;
    process_t *process_to_run = NULL;
    node_t *node = NULL;

    int num_cycles = 0;
    int simulation_time = 0;
    int num_processes  = processes_from_file->size;
    int num_proc_left = -1;

    int total_execution_time = 0;
    double process_turnaround_time = 0;
    double process_time_overhead = 0;
    double total_time_overhead = 0;
    double max_time_overhead = -1;

    int process_manager_started = 0;

    // Run process manager until there are no more processes in the input queue and no READY or RUNNING processes.
    while (!process_manager_started||num_proc_from_file_left>0||!(is_empty(input_queue) && is_empty(ready_queue) && current_process==NULL)){
        process_manager_started = 1;

        // Add processes to input queue if their arrival time is less than or equal to the current simulation time
        node = processes_from_file->front;
        int process_added_to_input_queue = 0;
        while (node!=NULL){
            if ((node->process->time_arrived<=simulation_time)){
                process_added_to_input_queue = 1;
                enqueue(input_queue, node->process);
                num_proc_from_file_left--;
            } 
            else {
                process_added_to_input_queue = 0;
            }

            node = node->next;

            if (process_added_to_input_queue){
                dequeue(processes_from_file);
            }
        }

        // Current process has finished running
        if (current_process && current_process->run_time >= current_process->service_time){
            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, current_process->process_name, num_proc_left);
            
            // Terminate real process
            char sha256[64+1];
            terminate_process(current_process, simulation_time, sha256);
            sha256[64] = '\0';
            printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s\n", simulation_time, current_process->process_name, sha256);

            // Free simulated process memory
            free_process_memory(memory, current_process);
            
            // Calculate performance statistics for process
            process_turnaround_time = simulation_time-current_process->time_arrived;
            total_execution_time += process_turnaround_time;
            process_time_overhead = process_turnaround_time/current_process->service_time;
            total_time_overhead += process_time_overhead;
            if (max_time_overhead==-1 || (process_time_overhead > max_time_overhead)){
                max_time_overhead = process_time_overhead;
            }
        } 

        // Allocate simulated memory to processes and add them to the ready queue
        int allocated_memory_address;
        node = input_queue->front;
        for (int i=0; i<input_queue->size; i++){
            // Prevent adding processes that are already in the queue or have been in the past
            if ((node->process->time_arrived<=simulation_time && node->process->state==NONE)){
                if (strcmp(memory_strategy, "best-fit")==0){
                    allocated_memory_address = allocate_process_memory(memory, node->process);

                    // If memory was successfully allocated to process, its state becomes READY
                    if (allocated_memory_address!=-1){
                        printf("%d,READY,process_name=%s,assigned_at=%d\n", simulation_time, node->process->process_name, allocated_memory_address);
                        node->process->state = READY;
                        node_t *next = node->next;
                        process_t *process = remove_from_queue(input_queue, node->process);
                        enqueue(ready_queue, process);

                        if (next==NULL){
                            break;
                        }
                        node = next;
                    }
                }
                else {
                    node->process->state = READY;
                    node_t *next = node->next;
                    enqueue(ready_queue, node->process);
                    remove_from_queue(input_queue, node->process);

                    if (next==NULL){
                        break;
                    }
                    node = next;
                }
            }
        }

        // Schedule the next process to be run
        process_to_run = schedule_process(ready_queue, scheduler, current_process);
        num_proc_left = input_queue->size + ready_queue->size;
        
        // Process to be run is just starting or resuming (thus different to current process)
        if (process_to_run!=NULL&&process_to_run!=current_process){
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", simulation_time, process_to_run->process_name, process_to_run->service_time - process_to_run->run_time);
            
            // Suspend current process if its state is not FINISHED
            if (current_process!=NULL&&current_process->state!=FINISHED){
                suspend_process(current_process, simulation_time);
            }

            // Different methods to control process about to run, depending on whether it is starting or resuming
            // If process is just starting and does not yet have a pid
            if (process_to_run->pid==-1){
                run_process(process_to_run, simulation_time);
            }
            // If resuming
            else {
                resume_process(process_to_run, simulation_time);
            }
            
        }
        // The same process is continued to run
        else if (process_to_run!=NULL){
            resume_process(process_to_run, simulation_time);
        }

        // Free process memory if it is finished
        if (current_process!=NULL && current_process->state==FINISHED){
            free(current_process);
            current_process = NULL;
        }
        
        // Switch from the current process to the next process, to be run in the next CPU cycle
        current_process = process_to_run;
        if (current_process!=NULL){
            current_process->state = RUNNING;
            current_process->run_time += quantum;
        }

        simulation_time += quantum;
        num_cycles++;
    }

    // Calculate performance statistics
    int turnaround_time = ceil((double)total_execution_time/num_processes);
    double avg_time_overhead = total_time_overhead/num_processes;

    // Print performance statistics
    printf("Turnaround time %d\n", turnaround_time);
    printf("Time overhead %.2f %.2f\n", max_time_overhead, avg_time_overhead);
    printf("Makespan %d\n", simulation_time-quantum);

    // Cleanup
    free(node);
    free(processes_from_file);
    free(input_queue);
    free(ready_queue);

    return 0;
}