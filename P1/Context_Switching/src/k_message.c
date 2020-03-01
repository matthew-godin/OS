#include "k_message.h"
#include "k_mailbox.h"
#include "k_rtx.h"
#include "k_process.h"
#include "k_process_priority_queue.h"

extern PCB* gp_current_process;
extern PCB* gp_pcb_message_waiting_queue[];
MAILBOX mailboxes[NUM_TEST_PROCS];

void init_mailboxes() {
    int i;
    for (i = 0; i < NUM_TEST_PROCS; ++i) {
        init_mailbox(&mailboxes[i]);
    }
}

int send_message(int process_id, void* message_envelope) {
	PCB* unblocked_pcb = NULL;
	MSG_BUF* incoming_message_envelope = (MSG_BUF*)message_envelope;

	incoming_message_envelope->m_send_pid = gp_current_process->m_pid; //set sender pid to current process's pid
	incoming_message_envelope->m_recv_pid = process_id; //set receiving pid to process id
	
	push_msg(&mailboxes[process_id-1], incoming_message_envelope); //not sure if we want to modify some more fields
	
	if (gp_pcb_message_waiting_queue[process_id-1] != NULL) { //process was waiting for message, wake up
		unblocked_pcb = gp_pcb_message_waiting_queue[process_id-1];
		gp_pcb_message_waiting_queue[process_id-1] = NULL; // remove from waiting
        unblocked_pcb->m_state = RDY;
        push_pcb_queue(unblocked_pcb);
	}
    k_release_processor();
	return RTX_OK;
}

void* receive_message(int* sender_id) {
	int process_id = gp_current_process->m_pid;
	if (is_empty(&mailboxes[process_id-1])) { //Could and probably should be an if statement
        k_release_blocked_processor(WAITING_MESSAGE);
	} else {
        k_release_processor();
    }
	return pop_msg(&mailboxes[process_id-1]);
}
