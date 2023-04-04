#include "process_manager.h"
#include "queue.h"

void shortest_job_first_scheduler(queue_t *ready_queue){
    int shortest_service_time = -1;
    process_t process;

    // iterate through queue
    for (int i; i<ready_queue->size; i++){
        // process = ready_queue process
        if (shortest_service_time==-1){ 
            shortest_service_time = process.service_time;
        }
        else if (process.service_time<shortest_service_time){
            // add process to running state
        }
    }
}

void round_robin_scheduler(queue_t *ready_queue){

}