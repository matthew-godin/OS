#include "k_memory_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

QUEUE push(QUEUE q, U8* addr) {
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

QUEUE pop(QUEUE q) { //if we pop that last block what happens?
	if(q.head == NULL) {
		return q;
	}
	q.head = (U8*)*(q.head); // :/
	return q;
}

U8* top(QUEUE q) { 
	return q.head;
}

int isEmpty(QUEUE q) {
	return q.head == NULL;
}

QUEUE initQueue(QUEUE q){
	q.head = NULL;
	q.tail = NULL;
	return q;
}
