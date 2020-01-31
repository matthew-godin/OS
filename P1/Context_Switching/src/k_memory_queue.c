#include "k_memory_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

MEMORY_QUEUE push(MEMORY_QUEUE q, U8* addr) {
	if(q.tail != NULL) {
		*(q.tail) = (*addr);
	}
	q.tail = addr;
	*addr = NULL;

	if(q.head == NULL) {
		q.head = addr;
	}
	return q;
}

MEMORY_QUEUE pop(MEMORY_QUEUE q) { //if we pop that last block what happens?
	if(q.head == NULL) {
		return q;
	}
	q.head = (U8*)*(q.head); // :/
	return q;
}

U8* top(MEMORY_QUEUE q) { 
	return q.head;
}

int isEmpty(MEMORY_QUEUE q) {
	return q.head == NULL;
}

MEMORY_QUEUE initQueue(MEMORY_QUEUE q){
	q.head = NULL;
	q.tail = NULL;
	return q;
}
