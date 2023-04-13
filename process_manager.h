#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "queue.h"

struct queue_t;

enum process_state {
    NONE,
    READY,
    RUNNING,
    FINISHED
};

typedef struct process_t {
    int time_arrived;
    char process_name[8];
    int service_time;
    int run_time;
    int memory_requirement;
    int memory_address;
    enum process_state state;
    pid_t pid;
    int fds[2][2];
} process_t;

process_t *create_process(int time_arrived, char* process_name, int service_time, int memory_requirement);
int run_process(process_t *process, int simulation_time);
int suspend_process(process_t *process, int simulation_time);
int resume_process(process_t *process, int simulation_time);
int terminate_process(process_t *process, int simulation_time, char *sha256);
void print_process(process_t *process);
process_t *schedule_process(struct queue_t *ready_queue, char *scheduler, process_t *current_process);
int allocate_process_memory(int *memory, process_t *process);
void free_process_memory(int *memory, process_t *process);

#endif