#include "k_memory_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

void push(queue* q, U32 addr) {
	node* node;
	node->addr = addr;
	node->next = q->tail;
	q->tail = node;
	
	if(q->head == NULL) {
		q->head = node;
	}
}

U32 pop(queue* q) {	
	if(isEmpty(q)) {
		return 0; // TODO: might need to change
	}
	U32 addr = q->head->addr;
	q->head = q->head->next;
	return addr;
}

bool isEmpty(queue* q) {
	return q->head == NULL;
}
