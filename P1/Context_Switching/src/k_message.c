#include "k_message.h"
#include "k_mailbox.h"
#include "k_rtx.h"

MAILBOX mailboxes[NUM_TEST_PROCS];

void init_mailboxes() {
    int i;
    for (i = 0; i < NUM_TEST_PROCS; ++i) {
        init_mailbox(&mailboxes[i]);
    }
}

int send_message(int process_id, void* message_envelope) {
}

void* receive_mesage(int* sender_id) {
}