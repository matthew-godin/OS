#ifndef K_MESSAGE_H_
#define K_MESSAGE_H_

void k_init_mailboxes(void);
int k_send_message(int process_id, void* message_envelope);
void* k_receive_message(int* sender_id);

#endif
