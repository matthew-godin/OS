#ifndef K_MEMORY_QUEUE_H_
#define K_MEMORY_QUEUE_H_

typedef struct node 

{
	NODE* next;
} NODE;

typedef struct queue 
{
	NODE* first;
	NODE* last;
} QUEUE;


#endif