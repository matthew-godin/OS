#ifndef K_PROCESS_PRIORITY_QUEUE_H_
#define K_PROCESS_PRIORITY_QUEUE_H_

#include "k_rtx.h"

typedef struct NODE {
	PCB pcb;
	NODE* next;
} NODE;

typedef struct PROCESS_PRIORITY_QUEUE {
	NODE* head;
	NODE* tail;
} PROCESS_PRIORITY_QUEUE;


#endif