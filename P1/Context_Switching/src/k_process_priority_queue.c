#include "k_process_priority_queue.h"
#ifdef DEBUG_0
#include "printf.h"
#endif
#include "common.h"

PCB* gp_pcb_queue[NUM_KERNEL_PROCS + NUM_TEST_PROCS + 1];
PCB* gp_pcb_waiting_memory_queue[NUM_KERNEL_PROCS + NUM_TEST_PROCS + 1];

void init_pcb_queue(void) {
	int i;
	for (i = 0; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1; ++i) {
		gp_pcb_queue[i] = NULL;
	}
}

void push_pcb_queue(PCB* pcb) {
	PCB* temp;
	int i;

	if (pcb->m_state != NEW && pcb->m_state != RDY) {
		#ifdef DEBUG_0
		printf("ERROR: tried to push a non ready pcb\r\n");
		#endif
		return;
	}

	if (gp_pcb_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] != NULL) { // Should never hit here
		#ifdef DEBUG_0
		printf("ERROR: tried to push more processes than exist to ready queue\n\r");
		#endif
		return;
	}
	gp_pcb_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] = pcb;

	for (i = NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1; i >= 1; --i) {
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
	for (i = 0; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1; ++i) {
		gp_pcb_queue[i] = gp_pcb_queue[i+1];
	}
	gp_pcb_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] = NULL;
	return pcb;
}

void updated_pcb_priority(int pid) {
	int i;
	int found = 0;
	PCB* updatedPCB;

	// Find the updated PCB
	for (i = 0; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1; ++i) {
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
	for (; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1; ++i) {
		gp_pcb_queue[i] = gp_pcb_queue[i+1];
	}
	gp_pcb_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] = NULL;

	// Push new pcb to array
	push_pcb_queue(updatedPCB);
}

void init_pcb_waiting_memory_queue(void) {
	int i;
	for (i = 0; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1; ++i) {
		gp_pcb_waiting_memory_queue[i] = NULL;
	}
}

void push_pcb_waiting_memory_queue(PCB* pcb) {
	PCB* temp;
	int i;

	if (pcb->m_state != WAITING_MEMORY) {
		#ifdef DEBUG_0
		printf("ERROR: tried to push a non waiting pcb to waiting memory queue\n\r");
		#endif
		return;
	}

	if (gp_pcb_waiting_memory_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] != NULL) { // Should never hit here
		#ifdef DEBUG_0
		printf("ERROR: tried to push more processes than exist to waiting memory queue\n\");
		#endif
		return;
	}
	gp_pcb_waiting_memory_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] = pcb; //push to end

	for (i = NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1; i >= 1; --i) { //bubble down the queue
		if (gp_pcb_waiting_memory_queue[i-1] == NULL || gp_pcb_waiting_memory_queue[i-1]->m_priority > gp_pcb_waiting_memory_queue[i]->m_priority) {
			temp = gp_pcb_waiting_memory_queue[i];
			gp_pcb_waiting_memory_queue[i] = gp_pcb_waiting_memory_queue[i-1];
			gp_pcb_waiting_memory_queue[i-1] = temp;
		} else {
			break;
		}
	}
}

PCB* pop_pcb_waiting_memory_queue(void) {
	int i;
	PCB* pcb = gp_pcb_waiting_memory_queue[0];
	for (i = 0; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1; ++i) {
		gp_pcb_waiting_memory_queue[i] = gp_pcb_waiting_memory_queue[i+1];
	}
	gp_pcb_waiting_memory_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] = NULL;
	return pcb;
}

void updated_pcb_waiting_memory_priority(int pid) {
	int found = 0;
	int i;
	PCB* updatedPCB;

	// Find the updated PCB
	for (i = 0; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 ; ++i) {
		if (gp_pcb_waiting_memory_queue[i]->m_pid == pid) {
			updatedPCB = gp_pcb_waiting_memory_queue[i];
			found = 1;
			break;
		}
	}

	if (!found) {
		return;
	}

	// "Remove" pcb from array
	for (; i < NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1; ++i) {
		gp_pcb_waiting_memory_queue[i] = gp_pcb_waiting_memory_queue[i+1];
	}
	gp_pcb_waiting_memory_queue[NUM_TEST_PROCS + NUM_KERNEL_PROCS + 1 -1] = NULL;

	// Push new pcb to array
	push_pcb_waiting_memory_queue(updatedPCB);
}

int pcb_waiting_memory_queue_is_empty() {
	if(gp_pcb_waiting_memory_queue[0] == NULL) {
		return 1;
	}
	return 0;
}
