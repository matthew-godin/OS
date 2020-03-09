#include "rtx.h"
#include "k_procs.h"
#include "k_timeout_queue.h"
#include "k_message.h"

PROC_INIT g_kernel_procs[NUM_KERNEL_PROCS];
extern TIMEOUT_QUEUE timeout_queue;

void set_kernel_procs() {
    g_kernel_procs[0].m_pid= 0;
    g_kernel_procs[0].m_priority= 4;
    g_kernel_procs[0].m_stack_size= 128;
    g_kernel_procs[0].mpf_start_pc= &null_proc;
}

void null_proc() {
    while(1) {
        release_processor();
    }
}

void timer_i_proc() {
    MSG_BUF* ready_message = NULL;
    MSG_BUF* new_message = k_receive_message(NULL);
    int pid = 0;

    while(new_message != NULL) {
        insert_tq_msg(&timeout_queue, new_message);
        new_message = k_receive_message(NULL);
    }

    if (!is_tq_empty(&timeout_queue)) {
        top_tq(&timeout_queue)->m_time -= 1;
    }
    while(!is_tq_empty(&timeout_queue) && top_tq(&timeout_queue)->m_time == 0) {
        ready_message = pop_tq_msg(&timeout_queue);
        pid = ready_message->m_recv_pid;
        k_send_message(pid, ready_message);
    }
}

void crt_proc() {
    MSG_BUF* receive_message = NULL;
    char* received_char;
    while(1) {
        receive_message = k_receive_message(NULL);
        //send it to UART1_IRQ
    }
}

void kcd_proc() {
    MSG_BUF* receive_message = NULL;
    while(1) {
        receive_message = k_receive_message(NULL);
    }
}