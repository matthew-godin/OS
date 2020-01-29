#include "k_memory_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void push(QUEUE* q) {
	NODE* node;
	node.next = NULL;
	q->tail.next = node;
	q.tail = node;
	
	if(q.head == NULL) {
		q.head = node;
	}
}

U8 pop(QUEUE* q) {	
	if(isEmpty(q)) {
		return 0; // TODO: might need to change
	}
	U8 addr = q.head;
	q->head = q->head->next;
	return addr;
}

bool isEmpty(QUEUE* q) {
	return q.head == NULL;
}

void initQueue(QUEUE* q){
	q.head = NULL;
	q.tail = NULL;
}
