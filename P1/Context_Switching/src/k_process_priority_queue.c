#include "k_process_priority_queue.h"
#include "printf.h"

PCB* gp_pcb_queue[NUM_TOTAL_PROCS];
PCB* gp_pcb_waiting_queue[NUM_TOTAL_PROCS];

void init_pcb_queue(void) {
	int i;
	for (i = 0; i < NUM_TOTAL_PROCS; ++i) {
		gp_pcb_queue[i] = NULL;
	}
}

void push_pcb_queue(PCB* pcb) {
	PCB* temp;
	int i;

	if (pcb->m_state != NEW && pcb->m_state != RDY) {
		printf("ERROR: tried to push a non ready pcb\n");
		return;
	}

	if (gp_pcb_queue[NUM_TOTAL_PROCS-1] != NULL) { // Should never hit here
		printf("ERROR: tried to push more processes than exist\n");
		return;
	}
	gp_pcb_queue[NUM_TOTAL_PROCS-1] = pcb;

	for (i = NUM_TOTAL_PROCS-1; i >= 1; --i) {
		if (gp_pcb_queue[i-1] == NULL || gp_pcb_queue[i-1]->m_priority > gp_pcb_queue[i]->m_priority) {
			temp = gp_pcb_queue[i];
			gp_pcb_queue[i] = gp_pcb_queue[i-1];
			gp_pcb_queue[i-1] = temp;
		} else {
			break;
		}
	}
}

PCB* pop_pcb_queue(void) {
	int i;
	PCB* pcb = gp_pcb_queue[0];
	for (i = 0; i < NUM_TOTAL_PROCS-1; ++i) {
		gp_pcb_queue[i] = gp_pcb_queue[i+1];
	}
	gp_pcb_queue[NUM_TOTAL_PROCS-1] = NULL;
	return pcb;
}

void updated_pcb_priority(int pid) {
	int i;
	int found = 0;
	PCB* updatedPCB;

	// Find the updated PCB
	for (i = 0; i < NUM_TOTAL_PROCS; ++i) {
		if (gp_pcb_queue[i]->m_pid == pid) {
			updatedPCB = gp_pcb_queue[i];
			found = 1;
			break;
		}
	}
	
	if (!found) {
		return;
	}
	
	// "Remove" pcb from array
	for (; i < NUM_TOTAL_PROCS-1; ++i) {
		gp_pcb_queue[i] = gp_pcb_queue[i+1];
	}
	gp_pcb_queue[NUM_TOTAL_PROCS-1] = NULL;

	// Push new pcb to array
	push_pcb_queue(updatedPCB);
}

void init_pcb_waiting_queue(void) {
	int i;
	for (i = 0; i < NUM_TOTAL_PROCS; ++i) {
		gp_pcb_waiting_queue[i] = NULL;
	}
}

void push_pcb_waiting_queue(PCB* pcb) {
	PCB* temp;
	int i;

	if (pcb->m_state != WAITING) {
		printf("ERROR: tried to push a non waitin pcb to waiting queue\n");
		return;
	}

	if (gp_pcb_waiting_queue[NUM_TOTAL_PROCS-1] != NULL) { // Should never hit here
		printf("ERROR: tried to push more processes than exist\n");
		return;
	}
	gp_pcb_waiting_queue[NUM_TOTAL_PROCS-1] = pcb; //push to end

	for (i = NUM_TOTAL_PROCS-1; i >= 1; --i) { //bubble down the queue
		if (gp_pcb_waiting_queue[i-1] == NULL || gp_pcb_waiting_queue[i-1]->m_priority > gp_pcb_waiting_queue[i]->m_priority) {
			temp = gp_pcb_waiting_queue[i];
			gp_pcb_waiting_queue[i] = gp_pcb_waiting_queue[i-1];
			gp_pcb_waiting_queue[i-1] = temp;
		} else {
			break;
		}
	}
}

PCB* pop_pcb_waiting_queue(void) {
	int i;
	PCB* pcb = gp_pcb_waiting_queue[0];
	for (i = 0; i < NUM_TOTAL_PROCS-1; ++i) {
		gp_pcb_waiting_queue[i] = gp_pcb_waiting_queue[i+1];
	}
	gp_pcb_waiting_queue[NUM_TOTAL_PROCS-1] = NULL;
	return pcb;
}

void updated_pcb_waiting_priority(int pid) {
	int found = 0;
	int i;
	PCB* updatedPCB;

	// Find the updated PCB
	for (i = 0; i < NUM_TOTAL_PROCS; ++i) {
		if (gp_pcb_waiting_queue[i]->m_pid == pid) {
			updatedPCB = gp_pcb_queue[i];
			found = 1;
			break;
		}
	}

	if (!found) {
		return;
	}

	// "Remove" pcb from array
	for (; i < NUM_TOTAL_PROCS-1; ++i) {
		gp_pcb_waiting_queue[i] = gp_pcb_waiting_queue[i+1];
	}
	gp_pcb_waiting_queue[NUM_TOTAL_PROCS-1] = NULL;

	// Push new pcb to array
	push_pcb_waiting_queue(updatedPCB);
}

int pcb_waiting_queue_is_empty() {
	if(gp_pcb_waiting_queue[0] == NULL) {
		return 1;
	} 
	return 0;
}



