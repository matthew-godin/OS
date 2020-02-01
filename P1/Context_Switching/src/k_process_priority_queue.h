#ifndef K_PROCESS_PRIORITY_QUEUE_H_
#define K_PROCESS_PRIORITY_QUEUE_H_

#include "k_rtx.h"

typedef struct NODE_T {
	PCB* pcb;
	struct NODE_T* next;
} NODE;

typedef struct PROCESS_QUEUE {
	NODE* head;
	NODE* tail;
} PROCESS_QUEUE;

PROCESS_QUEUE push_pq(PROCESS_QUEUE q, PCB* pcb);
PROCESS_QUEUE pop_pq(PROCESS_QUEUE q);
PCB* top_pq(PROCESS_QUEUE q);
PROCESS_QUEUE init_pq(PROCESS_QUEUE q);

typedef struct PROCESS_PRIORITY_QUEUE {
	PROCESS_QUEUE pqs[5];
} PROCESS_PRIORITY_QUEUE;

PROCESS_PRIORITY_QUEUE init_ppq(PROCESS_PRIORITY_QUEUE ppq);
PROCESS_PRIORITY_QUEUE push_ppq(PROCESS_PRIORITY_QUEUE ppq,  PCB* pcb);
PROCESS_PRIORITY_QUEUE pop_ppq(PROCESS_PRIORITY_QUEUE ppq);
PCB* top_ppq(PROCESS_PRIORITY_QUEUE ppq);

void init_pcb_queue(void);

void push_pcb_queue(PCB* pcb);

PCB* pop_pcb_queue(void);

#endif
