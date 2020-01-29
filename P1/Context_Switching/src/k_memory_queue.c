#include "k_memory_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void push(QUEUE* q, U8* addr) {
	if(q.tail != NULL ) {
		q->tail = addr;
	}
	q.tail = addr;
	
	if(q.head == NULL) {
		q.head = addr;
	}
}

U8* pop(QUEUE* q) {
	if(q.head == NULL) {
		return NULL;
	}
	
	U8* returnAddr = q.head;
	q.head = q->head;
	return returnAddr;
}

bool isEmpty(QUEUE* q) {
	return q.head == NULL;
}

void initQueue(QUEUE* q){
	q.head = NULL;
	q.tail = NULL;
}
