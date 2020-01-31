#include "k_process_priority_queue.h"

PROCESS_QUEUE push_pq(PROCESS_QUEUE q, PCB* pcb) {
	NODE* newNode;
	newNode->pcb = pcb;
	newNode->next = NULL;
	
	if(q.tail == NULL) {
		q.tail = newNode;
		q.head = newNode;
	} else {
		q.tail->next = newNode;
		q.tail = newNode;
	}	
	return q;
}
PROCESS_QUEUE pop_pq(PROCESS_QUEUE q) {
	if(q.head == NULL) {
		return q;
	}
	q.head = q.head->next;
	return q;
}
PCB* top_pq(PROCESS_QUEUE q) {
	if(q.head == NULL) {
		return NULL;
	}
	return q.head->pcb;
}
PROCESS_QUEUE init_pq(PROCESS_QUEUE q) {
	q.head = NULL;
	q.tail = NULL;
	return q;
}

PROCESS_PRIORITY_QUEUE init_ppq(PROCESS_PRIORITY_QUEUE ppq) {
	int i;
	for(i = 0; i < 5; ++i) {
		PROCESS_QUEUE q; 
		ppq.pqs[i] = init_pq(q);
	}
	return ppq;
}

PROCESS_PRIORITY_QUEUE push_ppq(PROCESS_PRIORITY_QUEUE ppq,  PCB* pcb) {
	int priority;
	PROCESS_QUEUE q;
	
	if(pcb->m_state != 1) return ppq; //if pcb isn't ready, don't push
	
	priority = pcb->m_priority;
	q = ppq.pqs[priority]; //get queue associated with pcb priority
	q = push_pq(q, pcb); //push pcb to queue associated with pcb priority
	ppq.pqs[priority] = q; //update ppq with updated queue
	return ppq;
}

PROCESS_PRIORITY_QUEUE pop_ppq(PROCESS_PRIORITY_QUEUE ppq) {
	int i;
	PROCESS_QUEUE q;
	
	for(i = 0; i < 5; ++i) {
		q = ppq.pqs[i];
		if(q.head !=NULL) {
			q = pop_pq(q);
			ppq.pqs[i] = q;
			return ppq;
		}			
	}
	
	return ppq;
}

PCB* top_ppq(PROCESS_PRIORITY_QUEUE ppq) {
	int i;
	PROCESS_QUEUE q;
	
	for(i = 0; i < 5; ++i) {
		q = ppq.pqs[i];
		if(q.head !=NULL) {
			return top_pq(q);
		}			
	}
	
	return NULL;
}

