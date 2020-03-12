#ifndef HOT_KEY_PROCS_H_
#define HOT_KEY_PROCS_H_
#include "k_rtx.h"

void print_ready_queue(void);
void print_blocked_memory_queue(void);
void print_blocked_message_queue(void);
void print_queue(PCB* queue[], int size);

#endif
