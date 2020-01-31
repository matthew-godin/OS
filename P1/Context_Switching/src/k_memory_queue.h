#ifndef K_MEMORY_QUEUE_H_
#define K_MEMORY_QUEUE_H_

#include "k_rtx.h"

typedef struct MEMORY_QUEUE 
{
	U8* head;
	U8* tail;
} MEMORY_QUEUE;

MEMORY_QUEUE push_mq(MEMORY_QUEUE gp_memory_queue, U8* addr);
MEMORY_QUEUE pop_mq(MEMORY_QUEUE q);
U8* top_mq(MEMORY_QUEUE q);
MEMORY_QUEUE init_mq(MEMORY_QUEUE q);

#endif
