#include "k_memory_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

MEMORY_QUEUE push_mq(MEMORY_QUEUE q, U32* addr) {
	if(q.tail != NULL) {
		*(q.tail) = (U32)addr; //Update current tail to point to new tail
	}
	q.tail = addr;
	*addr = NULL;

	if(q.head == NULL) { //If pushed block is the first block of queue set head
		q.head = addr;
	}
	return q;
}

MEMORY_QUEUE pop_mq(MEMORY_QUEUE q) {
	if(q.head == NULL) {
		return q;
	}
	q.head = (U32*)*(q.head); // Dereference the ptr at head to get the next mem block its pointing to

	if (q.head == NULL) { // If head is null at this point we know the queue is empty so update tail to null
		q.tail = NULL;
	}
	return q;
}

U32* top_mq(MEMORY_QUEUE q) { 
	return q.head;
}

int isEmpty(MEMORY_QUEUE q) {
	return q.head == NULL;
}

MEMORY_QUEUE init_mq(MEMORY_QUEUE q){
	q.head = NULL;
	q.tail = NULL;
	return q;
}
