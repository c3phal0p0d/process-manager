#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "queue.h"
#include "process_manager.h"

queue_t *initialize_queue(){
    queue_t *queue = malloc(sizeof(queue_t));
    queue->size = 0;
    queue->front = NULL;
    queue->rear = NULL;

    return queue;
}

node_t *create_node(process_t *process){
    node_t *node = malloc(sizeof(node_t));
    node->process = process;
    node->next = NULL;

    return node;
}

void free_node(node_t *node){
    free(node->process);
    free(node->next);
    free(node);
}

int is_empty(queue_t *queue){
    return (queue->front==NULL);
}

// Implementation of the enqueue operation which adds a new process to the end of the queue
void enqueue(queue_t *queue, process_t *process){
    node_t *node = create_node(process);

    if (is_empty(queue)){
        queue->front = node;
        queue->rear = node;
        queue->size++;
        return;
    }

    queue->rear->next = node;
    queue->rear = node;
    queue->size++;
}

// Implementation of the dequeue operation which removes the first process in the queue and returns it
process_t *dequeue(queue_t *queue){
    if (is_empty(queue)){
        return NULL;
    }

    // Remove node from front of queue, making the next node the new front node
    node_t *tmp = queue->front;
    process_t *process = tmp->process;
    queue->front = queue->front->next;

    // Queue is empty after dequeue operation
    if (queue->front==NULL){
        queue->rear=NULL;
    }

    queue->size--;

    free(tmp);
    tmp = NULL;

    return process;
}

// Method to remove a specified process from any position in the queue
process_t *remove_from_queue(queue_t *queue, process_t *process){
    node_t *tmp = queue->front;
    node_t *prev = NULL;
    for (int i=0; i<queue->size; i++){
        if (tmp->process==process){
            // Front of queue
            if (prev==NULL){
                queue->front = tmp->next;
            }
            // End of queue
            else if (tmp->next==NULL){
                prev->next = NULL;
                queue->rear = prev;
            }
            else {
                prev->next = tmp->next;
            }

            process_t *process = tmp->process;
            free(tmp);

            queue->size--;

            return process;
        }

        prev = tmp;
        tmp = tmp->next;
    }

    free(tmp);

    // process not found in queue
    return NULL;
}

void print_queue(queue_t *queue){
    if (is_empty(queue)){
        printf("NULL\n");
        return;
    }

    node_t *tmp = queue->front;
    for (int i=0; i<queue->size; i++){
        if (tmp == NULL){
            printf("NULL\n");
            return;
        } 
        else {
            print_process(tmp->process);
            tmp = tmp->next;
        }
    }

    free(tmp);
}

void free_queue(queue_t *queue){
    if (!is_empty(queue)){
        node_t *node = queue->front;
        node_t *tmp;
        while (!is_empty(queue)){
            tmp = node;
            node = node->next;
        }

        free_node(tmp);
        free(tmp);
        free_node(node);
        free(node);
        free(queue->front);
        free(queue->rear);
        free(queue);

        return;
    }

    free(queue->front);
    free(queue->rear);
}

