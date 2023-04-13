#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
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
    int num_proc_from_file_left = processes_from_file->size;

    // printf("*** Processes from file ***\n");
    // print_queue(processes_from_file);

    int memory[2048] = {0};

    queue_t *input_queue = initialize_queue();
    queue_t *ready_queue = initialize_queue();
    process_t *current_process = NULL;
    process_t *process_to_run = NULL;

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
    while ((!process_manager_started||num_proc_from_file_left>0||!(is_empty(input_queue) && is_empty(ready_queue) && current_process==NULL))){   // !(is_empty(input_queue) && is_empty(ready_queue) && current_process==NULL)
        //printf("%d\n", num_proc_from_file_left);
        process_manager_started = 1;
        // printf("Cycle %d\n", num_cycles);
        
        // printf("*** CYCLE INFO ***\n");
        // printf("Cycle: %d\n", num_cycles);
        // printf("Simualation time: %d\n", simulation_time);

        // Add processes to input queue if their arrival time is less than or equal to the current simulation time
        // print_queue(processes_from_file);
        node_t *node = processes_from_file->front;
        
        // printf("Process: ");
        // print_process(node->process);

        while (node!=NULL){
            // printf("time arrived: %d\n", node->process->time_arrived);

            if ((node->process->time_arrived<=simulation_time)){// && node->process->time_arrived>(simulation_time - quantum)){
                enqueue(input_queue, node->process);
                dequeue(processes_from_file);
                // print_process(node->process);
                num_proc_from_file_left--;
                // printf("processes file queue:\n");
                // print_queue(processes_from_file);
            }

            node = node->next;
        }
        
        // printf("input queue size: %d\n", input_queue->size);

        //num_proc_left = input_queue->size + ready_queue->size;

        // Current process has finished running
        if (current_process && current_process->run_time >= current_process->service_time){
            printf("%d,FINISHED,process_name=%s,proc_remaining=%d\n", simulation_time, current_process->process_name, num_proc_left);
            
            // Terminate real process
            //char* sha256 = terminate_process(current_process, simulation_time);
            //printf("%d,FINISHED-PROCESS,process_name=%s,sha=%s\n", simulation_time, current_process->process_name, sha256);

            // Free process memory
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

        // Allocate memory to processes and add them to the ready queue
        int allocated_memory_address;
        node = input_queue->front;
        for (int i=0; i<input_queue->size; i++){
            // Prevent adding processes that are already in the queue or have been in the past
            if ((node->process->time_arrived<=simulation_time && node->process->state==NONE)){
                if (strcmp(memory_strategy, "best-fit")==0){
                    allocated_memory_address = allocate_process_memory(memory, node->process);
                    //printf("memory address: %d\n", allocated_memory_address);

                    // If memory was successfully allocated to process, its state becomes READY
                    if (allocated_memory_address!=-1){
                        printf("%d,READY,process_name=%s,assigned_at=%d\n", simulation_time, node->process->process_name, allocated_memory_address);
                        node->process->state = READY;
                        enqueue(ready_queue, node->process);
                        remove_from_queue(input_queue, node->process);

                        if (node->next==NULL){
                            break;
                        }
                        node = node->next;
                    }
                }
                else {
                    node->process->state = READY;
                    enqueue(ready_queue, node->process);
                    remove_from_queue(input_queue, node->process);

                    if (node->next==NULL){
                        break;
                    }
                    node = node->next;

                    // printf("Process: ");
                    // print_process(node->process);
                }
            }
        }

        // if (simulation_time>10 && simulation_time<40){
        //     printf("Simulation time: %d\n", simulation_time);
        //     printf("*** Processes from file ***\n");
        //     print_queue(processes_from_file);
        //     printf("*** Input queue ***\n");
        //     print_queue(input_queue);
        //     printf("*** Ready queue ***\n");
        //     print_queue(ready_queue);
        // }

        // if (simulation_time==174){
        //     printf("*** Ready queue ***\n");
        //     print_queue(ready_queue);
        // }

        // num_proc_left = input_queue->size + ready_queue->size;
        // printf("input queue size: %d\n", input_queue->size);
        // printf("ready queue size: %d\n", ready_queue->size);
        // printf("proc_remaining: %d\n", num_proc_left);

        process_to_run = schedule_process(ready_queue, scheduler, current_process);
        // print_process(process_to_run);
        num_proc_left = input_queue->size + ready_queue->size;
        
        // Process to be run is just starting or resuming (thus different to current process)
        if (process_to_run!=NULL&&process_to_run!=current_process){
            printf("%d,RUNNING,process_name=%s,remaining_time=%d\n", simulation_time, process_to_run->process_name, process_to_run->service_time - process_to_run->run_time);
            
            // Suspend current process
            //suspend_process(current_process, simulation_time);

            // Different methods to control real process about to run, depending on whether it is starting or resuming
            // If process is just starting and does not yet have a pid
            if (process_to_run->pid==-1){
                //run_process(process_to_run, simulation_time);
            }
            // If resuming
            else {
                //resume_process(process_to_run, simulation_time);
            }
            
        }

        // Schedule next process to run
        current_process = process_to_run; //schedule_process(ready_queue, scheduler, current_process);
        if (current_process!=NULL){
            current_process->state = RUNNING;
            current_process->run_time += quantum;
        }

        simulation_time += quantum;
        num_cycles++;

        // printf("process: ");
        // print_process(current_process);
    }

    int turnaround_time = ceil((double)total_execution_time/num_processes);
    double avg_time_overhead = total_time_overhead/num_processes;

    // Print performance statistics
    printf("Turnaround time %d\n", turnaround_time);
    printf("Time overhead %.2f %.2f\n", max_time_overhead, avg_time_overhead);
    printf("Makespan %d\n", simulation_time-quantum);

    // Cleanup
    free(processes_from_file);
    free(input_queue);
    free(ready_queue);

    return 0;
}