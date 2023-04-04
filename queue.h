#ifndef QUEUE_H
#define QUEUE_H

#include "process_manager.h"

typedef struct node_t {
    process_t process;
    struct node_t *next;
} node_t;

typedef struct queue_t {
    int size;
    node_t *front;
    node_t *rear;
} queue_t;

queue_t *initialize_queue();
int is_empty(queue_t *queue);
void enqueue(queue_t *queue, process_t process);
node_t *dequeue(queue_t *queue);
void print_queue(queue_t *queue);
void free_queue(queue_t *queue);

#endif