#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

typedef struct process_t {
    int time_arrived;
    char process_name[8];
    int service_time;
    int memory_requirement;
} process_t;

#endif