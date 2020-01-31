#ifndef K_MEMORY_QUEUE_H_
#define K_MEMORY_QUEUE_H_

#include "k_rtx.h"

typedef struct MEMORY_QUEUE 
{
	U8* head;
	U8* tail;
} MEMORY_QUEUE;

MEMORY_QUEUE push(MEMORY_QUEUE gp_memory_queue, U8* addr);
MEMORY_QUEUE pop(MEMORY_QUEUE q);
U8* top(MEMORY_QUEUE q);
MEMORY_QUEUE initQueue(MEMORY_QUEUE q);

#endif
