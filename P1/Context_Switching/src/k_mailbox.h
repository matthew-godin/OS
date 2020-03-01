#ifndef K_MAILBOX_H_
#define K_MAILBOX_H_

#include "common.h"
//typedef struct msgbuf msgbuf;

typedef struct MAILBOX
{
    MSG_BUF* head;
    MSG_BUF* tail;
} MAILBOX;

void push_msg(MAILBOX* mailbox, MSG_BUF* msg);
MSG_BUF* pop_msg(MAILBOX* mailbox);
void init_mailbox(MAILBOX* mailbox);

#endif