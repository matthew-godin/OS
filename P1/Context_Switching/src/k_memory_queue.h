#ifndef K_MEMORY_QUEUE_H_
#define K_MEMORY_QUEUE_H_

typedef struct queue 
{
	U8* first;
	U8* last;
} QUEUE;

void push(QUEUE* q, U8* addr);
U8* pop(QUEUE* q);
void initQueue(QUEUE* q);

#endif