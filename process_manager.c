#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
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
    process->pid = -1;

    return process;
}


void print_process(process_t *process){
    if (process==NULL){
        printf("NULL\n");
        return;
    }
    printf("time arrived: %d, ", process->time_arrived);
    printf("process name: %s, ", process->process_name);
    printf("service time: %d, ", process->service_time);
    printf("memory_requirement: %d, ", process->memory_requirement);
    printf("memory address: %d, ", process->memory_address);
    printf("pid: %d, \n", process->pid);
}

process_t *schedule_process(queue_t *ready_queue, char *scheduler, process_t *current_process){
    process_t* process_to_run = NULL;
    // printf("scheduling...\n");

    // Schedule process according to the Shortest Job First algorithm
    if (strcmp(scheduler, "SJF")==0){
        // printf("sjf\n");
        // If current process has not yet finished, it will continue to run
        if (current_process!=NULL && current_process->service_time > current_process->run_time){
            return current_process;
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
            if (shortest_service_time==-1 || node->process->service_time<shortest_service_time || (node->process->service_time==shortest_service_time && node->process->time_arrived<process_to_run->time_arrived)
                    || (node->process->service_time==shortest_service_time && node->process->time_arrived==process_to_run->time_arrived && strcmp(node->process->process_name, process_to_run->process_name)<0)){
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
        process_to_run = remove_from_queue(ready_queue, process_to_run);
        process_to_run->state = RUNNING;
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

/************************************************************/
/* Functions for controlling real processes, used in task 4 */
/************************************************************/
void convert_int_to_hex(int num, unsigned char *hex){
    // Adapted from https://stackoverflow.com/questions/3784263/converting-an-int-into-a-4-byte-char-array-c
    hex[0] = (num >> 24) & 0xFF;
    hex[1] = (num >> 16) & 0xFF;
    hex[2] = (num >> 8) & 0xFF;
    hex[3] = num & 0xFF;

    //printf("%x %x %x %x\n", hex[0], hex[1], hex[2], hex[3]);
}

int run_process(process_t *process, int simulation_time){
    //printf("running process...\n");

    // Set up pipes for two-way communication
    int fd1[2], fd2[2];
    pipe(fd1);
    pipe(fd2);
    // Store pipes in corresponding process struct so it can be used later to communicate with the same process
    process->fds[0][0] = fd1[0];
    process->fds[0][1] = fd1[1];
    process->fds[1][0] = fd2[0];
    process->fds[1][1] = fd2[1];

    // Fork child process
    pid_t root_pid = getpid();
    pid_t process_pid;
    if ((process_pid = fork()) == -1){
        perror("fork error\n");
        exit(1);
    }
    
    // Child process
    if (getpid()!=root_pid){
        //printf("in child process, pid: %d\n", getpid());

        process->pid = getpid();

        // Set up pipes for communication with process manager
        close(fd1[0]);                  // close pipe1 read side
        dup2(fd1[1], STDOUT_FILENO);    // redirect stdout
        close(fd2[1]);                  // close pipe2 write side
        dup2(fd2[0], STDIN_FILENO);     // redirect stdin

        //close(fd1[1]);
        //close(fd2[0]);

        // Create instance of process
        char *args[] = {"", process->process_name, NULL};
        if (execvp("./process", args)==-1){
            perror("exec error\n");
            exit(1);
        }

    }
    // Parent process
    else {
        //printf("in parent process, pid: %d\n", getpid());

        //printf("process pid: %d\n", process_pid);
        process->pid = process_pid;

        // Convert simulation time from integer to 4-byte hex reprentation
        unsigned char hex[4];
        convert_int_to_hex(simulation_time, hex);

        //printf("simulation time: %d\n", simulation_time);
        //printf("in hex: %s\n", hex);

        // Make copies of stdin and stdout to be restored later
        int stdin_copy = dup(STDIN_FILENO);
        int stdout_copy = dup(STDOUT_FILENO);
        
        // Set up pipes for communication with process instance
        close(fd1[1]);                  // close pipe1 write side
        dup2(fd1[0], STDIN_FILENO);     // redirect stdin
        close(fd2[0]);                  // close pipe2 read side
        dup2(fd2[1], STDOUT_FILENO);    // redirect stdout

        // Send 32 bit simulation time of when process is started to standard input of process
        write(fd2[1], hex, sizeof(hex));

        //sleep(1);

        // Read 1 byte from standard output of process
        char process_output[8];
        read(fd1[0], process_output, sizeof(process_output));

        // Switch back to standard input/output
        //close(fd1[0]);  // close pipe1 read side
        //close(fd2[1]);  // close pipe2 write side
        dup2(stdin_copy, STDIN_FILENO);
        dup2(stdout_copy, STDOUT_FILENO);
        //printf("process output: %s\n", process_output);

        // Verify that the byte read is the same as the last byte that was sent
        if (process_output[0]==hex[3]){
            //printf("same\n");
            return 0;
        }

        return -1;
    }

    return -1;
}

int suspend_process(process_t *process, int simulation_time){
    //printf("suspending process...\n");

    // Convert simulation time from integer to 4-byte hex reprentation
    unsigned char hex[4];
    convert_int_to_hex(simulation_time, hex);

    // Make copies of stdin and stdout to be restored later
    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);
    
    // Set up pipes for communication with process instance
    //close(fd1[1]);                  // close pipe1 write side
    dup2(process->fds[0][0], STDIN_FILENO);     // redirect stdin
    //close(fd2[0]);                  // close pipe2 read side
    dup2(process->fds[1][1], STDOUT_FILENO);    // redirect stdout

    // Send 32 bit simulation time of when process is suspended to standard input of process
    write(process->fds[1][1], hex, sizeof(hex));

    //sleep(1);

    // Switch back to standard input/output
    //close(fd1[0]);  // close pipe1 read side
    //close(fd2[1]);  // close pipe2 write side
    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    //printf("back\n");
    
    // Send SIGSTP signal to process
    kill(process->pid, SIGTSTP);

    int wstatus;
    waitpid(process->pid, &wstatus, WUNTRACED); 
    if (WIFSTOPPED(wstatus)) {
        return 0;
    }

    return -1;
}

int resume_process(process_t *process, int simulation_time){
    //printf("resuming process...\n");

    // Convert simulation time from integer to 4-byte hex representation
    unsigned char hex[4];
    convert_int_to_hex(simulation_time, hex);

    // Make copies of stdin and stdout to be restored later
    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);
    
    // Set up pipes for communication with process instance
    //close(fd1[1]);                  // close pipe1 write side
    dup2(process->fds[0][0], STDIN_FILENO);     // redirect stdin
    //close(fd2[0]);                  // close pipe2 read side
    dup2(process->fds[1][1], STDOUT_FILENO);    // redirect stdout
    
    // Send 32 bit simulation time of when process is resumed to standard input of process
    write(process->fds[1][1], hex, sizeof(hex));

    //sleep(1);

    // Send SIGCONT signal to process
    kill(process->pid, SIGCONT);

    // Read 1 byte from standard output of process
    char process_output[8];
    read(process->fds[0][0], process_output, sizeof(process_output));

    // Switch back to standard input/output
    //close(fd1[0]);  // close pipe1 read side
    //close(fd2[1]);  // close pipe2 write side
    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    //printf("process output: %s\n", process_output);

    // Verify that the byte read is the same as the last byte that was sent
    if (process_output[0]==hex[3]){
        return 0;
    }

    return -1;
}

int terminate_process(process_t *process, int simulation_time, char *sha256){
    //printf("terminating process...\n");

    // Convert simulation time from integer to 4-byte hex reprentation
    unsigned char hex[4];
    convert_int_to_hex(simulation_time, hex);

    // printf("simulation time: %d\n", simulation_time);
    // printf("in hex: %x\n", hex);

    // Make copies of stdin and stdout to be restored later
    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);
    
    // Set up pipes for communication with process instance
    //close(fd1[1]);                  // close pipe1 write side
    dup2(process->fds[0][0], STDIN_FILENO);     // redirect stdin
    //close(fd2[0]);                  // close pipe2 read side
    dup2(process->fds[1][1], STDOUT_FILENO);    // redirect stdout

    // Send 32 bit simulation time of when process is finished to standard input of process
    write(process->fds[1][1], hex, sizeof(hex));

    //sleep(1);
    
    // Send SIGTERM signal to process
    kill(process->pid, SIGTERM);
    
    // Read 64 byte string from output of process
    read(process->fds[0][0], sha256, 64);

    // Switch back to standard input/output
    // close(process->fds[0][0]);  // close pipe1 read side
    // close(process->fds[1][1]);  // close pipe2 write side
    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    //printf("sha256: %s\n", sha256);

    return 0;
}