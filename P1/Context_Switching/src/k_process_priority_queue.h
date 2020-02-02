#ifndef K_PROCESS_PRIORITY_QUEUE_H_
#define K_PROCESS_PRIORITY_QUEUE_H_

#include "k_rtx.h"

void init_pcb_queue(void);

void push_pcb_queue(PCB* pcb);

PCB* pop_pcb_queue(void);

void updated_pcb_priority(int pid);

void init_pcb_waiting_queue(void);

void push_pcb_waiting_queue(PCB* pcb);

PCB* pop_pcb_waiting_queue(void);

void updated_pcb_waiting_priority(int pid);

int pcb_waiting_queue_is_empty(void);

#endif
