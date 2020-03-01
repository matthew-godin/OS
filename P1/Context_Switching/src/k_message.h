#ifndef K_MESSAGE_H_
#define K_MESSAGE_H_

int send_message(int process_id, void* message_envelope);
void* receive_message(int* sender_id);

#endif
