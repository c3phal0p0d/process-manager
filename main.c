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

    printf("*** Args ***\n");
    printf("%s, %s, %s, %d\n", filename, scheduler, memory_strategy, quantum);

    queue_t *input_queue = initialize_queue();
    read_file(filename, input_queue);

    printf("*** Input queue ***\n");
    print_queue(input_queue);

    // Allocate memory to processes and add them to the ready queue
    queue_t *ready_queue = initialize_queue();
    node_t *node = input_queue->front;
    for (int i=0; i<input_queue->size; i++){
        allocate_memory(node->process, memory_strategy);
        node->process->state = READY;
        enqueue(ready_queue, node->process);
        node = node->next;
    }

    printf("*** Ready queue ***\n");
    print_queue(ready_queue);

    // Schedule processes
    if (scheduler=="SJF"){
        shortest_job_first_scheduler(ready_queue);
    } else if (scheduler=="RR"){
        round_robin_scheduler(ready_queue);
    }

    return 0;
}