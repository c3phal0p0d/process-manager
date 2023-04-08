#ifndef UTILS_H
#define UTILS_H

#include "process_manager.h"

int read_file(char *filename, queue_t *input_queue);
int allocate_memory(process_t *process, char *memory_strategy);

#endif