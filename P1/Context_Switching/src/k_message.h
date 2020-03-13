#ifndef K_MESSAGE_H_
#define K_MESSAGE_H_

void mailboxes_init(void);
void timeout_queue_init(void);
int k_send_message(int process_id, void* message_envelope);
int k_delayed_send(int process_id, void* message_envelope, int delay);
void* k_receive_message(int* sender_id);
void* k_receive_message_non_blocking(int* sender_id);

#endif
