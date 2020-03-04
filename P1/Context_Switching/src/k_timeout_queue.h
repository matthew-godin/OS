#ifndef K_TIMEOUT_QUEUE_H_
#define K_TIMEOUT_QUEUE_H_

#include "common.h"

typedef struct TIMEOUT_QUEUE
{
  MSG_BUF* head;
  MSG_BUF* tail;
} TIMEOUT_QUEUE;

void insert_msg(TIMEOUT_QUEUE* timeout_queue, MSG_BUF* msg);
MSG_BUF* top_msg(TIMEOUT_QUEUE* timeout_queue);

void init_timeout_queue(TIMEOUT_QUEUE* timeout_queue);
int is_empty(TIMEOUT_QUEUE* timeout_queue);

#endif
