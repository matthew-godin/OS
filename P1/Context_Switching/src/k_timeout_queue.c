#include "k_timeout_queue.h"

/**
25, 10, 0, 15 <- how it looks in the timeoutQueue
25, 35, 35, 50 <- how the times actually look

insert(38)
25, 10, 0, 3, 12
25, 35, 35, 38, 50

*/

void insert_msg(TIMEOUT_QUEUE* timeout_queue, MSG_BUF* msg) {
  MSG_BUF* iter;
  int cur_time;
  int msg_time = msg->m_time;

  msg->mp_next = NULL; //reset mp_next pointer

  //empty queue
  if(is_empty(timeout_queue)) {
    timeout_queue->head = msg;
    timeout_queue->tail = msg;
    return;
  }

  //insert at front of queue
  if(msg_time <= timeout_queue->head->m_time) {
    timeout_queue->head->m_time-= msg_time; //update head time to be relative to msg_time
    msg->mp_next = timeout_queue->head; //point msg->next = head
    timeout_queue->head = msg; //update head
    return;
  }

  //insert at end of queue
  if(msg_time > timeout_queue->tail->m_time) {
    msg_time-= timeout_queue->tail->m_time; //update msg_time to be relative to tail time
    msg->m_time = msg_time; //update message time
    timeout_queue->tail->mp_next = msg; //set tail->next to point to msg
    timeout_queue->tail = msg; //update tail
    return;
  }

  //insert in middle of queue, in order
  //this is similar to the standard implementations found online for inserting into sorted linkedlist

  //find iter right before insert
  iter = timeout_queue->head;
  cur_time = timeout_queue->head->m_time; //set time to the head time
  while(iter->mp_next != NULL &&
    (cur_time +  ((MSG_BUF*) iter->mp_next)->m_time) < msg_time) {
    cur_time += ((MSG_BUF*) iter->mp_next)->m_time;
    iter = iter->mp_next;
  }

  msg_time-= cur_time; //update msg_time to be relative to previous time
  msg->mp_next = iter->mp_next; //point msg to iter->next;
  ((MSG_BUF*)msg->mp_next)->m_time-= msg_time; //update msg->next's time to be relative to msg_time
  iter->mp_next = msg;
  return;

}

MSG_BUF* top(TIMEOUT_QUEUE* timeout_queue) {
  return timeout_queue->head;
}

MSG_BUF* pop_msg(TIMEOUT_QUEUE* timeout_queue) {
    MSG_BUF* head;
	if(timeout_queue->head == NULL) {
		return NULL;
	}
  head = timeout_queue->head;
	timeout_queue->head = (MSG_BUF*) timeout_queue->head->mp_next; // Dereference the ptr at head to get the next mem block its pointing to

	if (timeout_queue->head == NULL) { // If head is null at this point we know the queue is empty so update tail to null
		timeout_queue->tail = NULL;
	}
	return head;
}

void init_timeout_queue(TIMEOUT_QUEUE* timeout_queue) {
  timeout_queue->head = NULL;
  timeout_queue->tail = NULL;
}

int is_empty(TIMEOUT_QUEUE* timeout_queue) {
  if (timeout_queue->head == NULL || timeout_queue->tail == NULL ) {
    return 1;
  }
  return 0;
}
