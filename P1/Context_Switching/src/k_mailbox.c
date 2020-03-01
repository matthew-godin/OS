#include "k_mailbox.h"
#include "common.h"


void push_msg(MAILBOX* mailbox, MSG_BUF* msg) {
	if (mailbox->tail != NULL) {
		mailbox->tail->mp_next = msg; //Update current tail to point to new tail
	}
	mailbox->tail = msg;
	msg->mp_next = NULL;

	if (mailbox->head == NULL) { //If pushed block is the first block of queue set head
		mailbox->head = msg;
	}
}

MSG_BUF* pop_msg(MAILBOX* mailbox) {
    MSG_BUF* head;
	if(mailbox->head == NULL) {
		return NULL;
	}
    head = mailbox->head;
	mailbox->head = (MSG_BUF*) mailbox->head->mp_next; // Dereference the ptr at head to get the next mem block its pointing to

	if (mailbox->head == NULL) { // If head is null at this point we know the queue is empty so update tail to null
		mailbox->tail = NULL;
	}
	return head;
}

void init_mailbox(MAILBOX* mailbox) {
    mailbox->head = NULL;
    mailbox->tail = NULL;
}

int is_empty(MAILBOX* mailbox) {
	return mailbox->head == NULL || mailbox->tail == NULL;
}
