#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

typedef struct process_t {
    int time_arrived;
    char process_name[8];
    int service_time;
    int memory_requirement;
} process_t;

void shortest_job_first_scheduler(queue_t *ready_queue);
void round_robin_scheduler(queue_t *ready_queue);

#endif