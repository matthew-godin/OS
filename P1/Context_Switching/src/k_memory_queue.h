#ifndef K_MEMORY_QUEUE_H_
#define K_MEMORY_QUEUE_H_

#include "k_rtx.h"

typedef struct QUEUE 
{
	U8* head;
	U8* tail;
} QUEUE;

QUEUE push(QUEUE gp_memory_queue, U8* addr);
QUEUE pop(QUEUE q);
U8* top(QUEUE q);
QUEUE initQueue(QUEUE q);

#endif
