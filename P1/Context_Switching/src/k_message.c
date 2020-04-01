#include "k_message.h"
#include "k_mailbox.h"
#include "k_rtx.h"
#include "k_process.h"
#include "k_process_priority_queue.h"
#include "k_timeout_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

extern PCB* gp_current_process;
extern PCB* gp_pcb_message_waiting_queue[];

int preemption_in_iproc = 0;

MAILBOX mailboxes[16];
TIMEOUT_QUEUE timeout_queue;

void mailboxes_init(void) {
    int i;
    for (i = 0; i < 16; ++i) {
        init_mailbox(&mailboxes[i]);
    }
}

void timeout_queue_init(void) {
	init_timeout_queue(&timeout_queue);
}

int k_send_message(int process_id, void* message_envelope) {
	PCB* unblocked_pcb = NULL;
	MSG_BUF* incoming_message_envelope = (MSG_BUF*)message_envelope;

  if(process_id < 0 || process_id > 15) {
    #ifdef DEBUG_0
    printf("Error process id doesn't exist\r\n");
    #endif /* DEBUG_0 */
    return NULL;
  }

  if( (process_id > 6 && process_id < 11) {
    #ifdef DEBUG_0
    printf("Error process id doesn't exist\r\n");
    #endif /* DEBUG_0 */
    return NULL;
  }


	incoming_message_envelope->m_send_pid = gp_current_process->m_pid; //set sender pid to current process's pid
	incoming_message_envelope->m_recv_pid = process_id; //set receiving pid to process id
	incoming_message_envelope->mp_next = NULL;

	push_msg(&mailboxes[process_id], incoming_message_envelope); //not sure if we want to modify some more fields

	if (gp_pcb_message_waiting_queue[process_id] != NULL) { //process was waiting for message, wake up
		unblocked_pcb = gp_pcb_message_waiting_queue[process_id];
		gp_pcb_message_waiting_queue[process_id] = NULL; // remove from waiting
        unblocked_pcb->m_state = RDY;
        push_pcb_queue(unblocked_pcb);
	}
	// Right now we release processor everytime but lab manual may suggest non
	// preempted send should keep running
	k_release_processor();
	return RTX_OK;
}

int i_send_message(int process_id, void* message_envelope) {
	PCB* unblocked_pcb = NULL;
	MSG_BUF* incoming_message_envelope = (MSG_BUF*)message_envelope;

	incoming_message_envelope->m_send_pid = gp_current_process->m_pid; //set sender pid to current process's pid
	incoming_message_envelope->m_recv_pid = process_id; //set receiving pid to process id
	incoming_message_envelope->mp_next = NULL;

	push_msg(&mailboxes[process_id], incoming_message_envelope); //not sure if we want to modify some more fields

	if (gp_pcb_message_waiting_queue[process_id] != NULL) { //process was waiting for message, wake up
		unblocked_pcb = gp_pcb_message_waiting_queue[process_id];
		gp_pcb_message_waiting_queue[process_id] = NULL; // remove from waiting
    unblocked_pcb->m_state = RDY;
    push_pcb_queue(unblocked_pcb);
		if(k_get_process_priority(process_id) < gp_current_process->m_priority) {
			preemption_in_iproc = 1;
		}
	}
	// Right now we release processor everytime but lab manual may suggest non
	// preempted send should keep running
	return RTX_OK;
}

int k_delayed_send(int process_id, void* message_envelope, int delay) {
	MSG_BUF* incoming_message_envelope;

  if(delay < 0) return RTX_ERR;

	incoming_message_envelope = (MSG_BUF*)message_envelope;
	incoming_message_envelope->m_send_pid = gp_current_process->m_pid;
	incoming_message_envelope->m_recv_pid = process_id;
	incoming_message_envelope->m_time = delay;

	//push_msg(&mailboxes[0], incoming_message_envelope);
		push_msg(&mailboxes[PID_TIMER_IPROC], incoming_message_envelope); //push to timer mailbox

	// same comment as above release processor
	k_release_processor();
	return RTX_OK;
}


void* k_receive_message_non_blocking(int* sender_id) {
	return pop_msg(&mailboxes[PID_TIMER_IPROC]); //pop from timer mailbox
}

void* k_receive_message(int* sender_id) {
	int process_id = gp_current_process->m_pid;

  if(process_id < 0 || process_id > 15) {
    #ifdef DEBUG_0
    printf("Error process id doesn't exist\r\n");
    #endif /* DEBUG_0 */
    return NULL;
  }

  if( (process_id > 6 && process_id < 11) {
    #ifdef DEBUG_0
    printf("Error process id doesn't exist\r\n");
    #endif /* DEBUG_0 */
    return NULL;
  }

	if (is_empty(&mailboxes[process_id])) { //Could and probably should be an if statement
        k_release_blocked_processor(WAITING_MESSAGE);
	} else {
        k_release_processor();
    }
	return pop_msg(&mailboxes[process_id]);
}
