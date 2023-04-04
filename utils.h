#ifndef UTILS_H
#define UTILS_H

#include "process_manager.h"

int read_file(char *filename, process_t *processes_list);
int allocate_memory(process_t process, char *memory_strategy);

#endif