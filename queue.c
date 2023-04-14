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
    //free_process(node->process);
    free(node->process);
    free_node(node->next);
    free(node->next);
    free(node);
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

process_t *dequeue(queue_t *queue){
    if (is_empty(queue)){
        return NULL;
    }

    node_t *tmp = queue->front;
    queue->front = queue->front->next;

    //process_t *process = malloc(sizeof(process_t));
    //process = tmp->process;

    if (queue->front==NULL){
        queue->rear=NULL;
    }

    queue->size--;
    //free(tmp);

    return tmp->process;
}

int remove_from_queue(queue_t *queue, process_t *process){
    // printf("removing from queue: ");
    // print_process(process);
    node_t *node = queue->front;
    node_t *prev_node = NULL;
    for (int i=0; i<queue->size; i++){
        if (node->process==process){
            if (prev_node==NULL){   // front of queue
                queue->front = node->next;
            }
            else if (node->next==NULL){
                prev_node->next = NULL;
                queue->rear = prev_node;
            }
            else {
                prev_node->next = node->next;
            }
            queue->size--;
            //free_node(node);
            //free(node);
            return 0;
        }
        prev_node = node;
        node = node->next;
    }

    return -1;  // process not found in queue
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

