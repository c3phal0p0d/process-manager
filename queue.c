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

int is_empty(queue_t *queue){
    return (queue->front==NULL);
}

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

node_t *dequeue(queue_t *queue){
    if (is_empty(queue)){
        return NULL;
    }

    node_t *front = queue->front;
    queue->front = queue->front->next;

    if (queue->front==NULL){
        queue->rear=NULL;
    }

    queue->size--;

    return front;
}

void print_queue(queue_t *queue){
    if (is_empty(queue)){
        printf("NULL\n");
        return;
    }

    node_t *node = queue->front;
    for (int i=0; i<queue->size; i++){
        if (node == NULL){
            printf("NULL\n");
            return;
        } 
        else {
            print_process(node->process);
            node = node->next;
        }
    }
}

void free_queue(queue_t *queue){
    assert(!is_empty(queue));

    node_t *node = queue->front;

    while (!is_empty(queue)){
        node = node->next;
        free(node);
    }

    free(node);
    free(queue);
}
