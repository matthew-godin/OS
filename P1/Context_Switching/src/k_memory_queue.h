#ifndef K_MEMORY_QUEUE_H_
#define K_MEMORY_QUEUE_H_

#include "k_rtx.h"

typedef struct QUEUE 
{
	U8* head;
	U8* tail;
} QUEUE;

void push(QUEUE* gp_memory_queue, U8* addr);
U8* pop(QUEUE* q);
void initQueue(QUEUE* q);

#endif
