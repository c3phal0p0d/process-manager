#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "queue.h"

struct queue_t;

enum process_state {
    READY,
    RUNNING,
    FINISHED
};

typedef struct process_t {
    int time_arrived;
    char process_name[8];
    int service_time;
    int memory_requirement;
    enum process_state state;
} process_t;

process_t *create_process(int time_arrived, char* process_name, int service_time, int memory_requirement);
void print_process(process_t *process);
void shortest_job_first_scheduler(struct queue_t *ready_queue);
void round_robin_scheduler(struct queue_t *ready_queue);

#endif