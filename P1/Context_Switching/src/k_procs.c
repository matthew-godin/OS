#include "rtx.h"
#include "k_procs.h"
#include "k_timeout_queue.h"
#include "k_message.h"
#include "k_command_map.h"
#include "hot_key_procs.h"

PROC_INIT g_kernel_procs[NUM_KERNEL_PROCS];
extern TIMEOUT_QUEUE timeout_queue;

MSG_BUF* kcd_buffer = NULL;
int kcd_buffer_index = 0;

void set_kernel_procs() {
    g_kernel_procs[0].m_pid= 0;
    g_kernel_procs[0].m_priority= 4;
    g_kernel_procs[0].m_stack_size= 128;
    g_kernel_procs[0].mpf_start_pc= &null_proc;

    g_kernel_procs[1].m_pid= 12;
    g_kernel_procs[1].m_priority= 0;
    g_kernel_procs[1].m_stack_size= 128;
    g_kernel_procs[1].mpf_start_pc= &kcd_proc;

    g_kernel_procs[2].m_pid= 13;
    g_kernel_procs[2].m_priority= 0;
    g_kernel_procs[2].m_stack_size= 128;
    g_kernel_procs[2].mpf_start_pc= &crt_proc;
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

//called after each character
void uart_i_proc(char c) {
   MSG_BUF* kcd_msg_env;

   //display character regardless
   MSG_BUF* crt_msg_env = (MSG_BUF*) k_request_memory_block(); //make this non-blocking

   crt_msg_env->mtype = CRT_DISPLAY;
   crt_msg_env->mtext[0] = c; //add character to be printed in message body
   k_send_message(PID_CRT, crt_msg_env);

   //handle hotkey
   if (c == '!') {
     print_ready_queue();
   } else if(c == '@') {
     print_blocked_memory_queue();
   } else if (c == '#') {
     print_blocked_message_queue();
   }

   //build message buffer for kcd display
   if(kcd_buffer == NULL) {
     kcd_buffer = (MSG_BUF*) k_request_memory_block(); //make this non-blocking
     kcd_buffer->mtype = KCD_CMD;
   }
   kcd_buffer->mtext[kcd_buffer_index++] = c; //put char on buffer, increment index

   if(c == '\n') { //last character
     kcd_msg_env = kcd_buffer; // set kcd_msg_evn to buffer
     kcd_buffer = NULL;  //flush the buffer
     kcd_buffer_index = 0; //reset index

     k_send_message(PID_KCD, kcd_msg_env); //send message to kcd
   }

}

void crt_proc() {
    MSG_BUF* receive_message = NULL;
    char* received_char;
    while(1) {
        receive_message = k_receive_message(NULL);
        //send it to UART1_IRQ
        k_release_memory_block(receive_message);
    }
}

//hot keys will not enter this code
//hot keys should be handled in uart i process
// this assumes that the message received ty the kcd is a COMLETE COMMAND,
// not just individual characters
void kcd_proc() {
    MSG_BUF* receive_message = NULL;
    char cmd;
    int pid;
    while(1) {
        receive_message = k_receive_message(NULL);
        if(receive_message != NULL) {
          cmd = receive_message->mtext[1];

          if(receive_message->mtype == KCD_REG) { //command registration
            if(receive_message->mtext[0] =='%') { //check if valid keyboard command
              insert_cmd(cmd, receive_message->m_send_pid);
            }
          } else { // invocation of command
            //send to output

            if(receive_message->mtext[0] =='%') {  //check if valid keyboard command
              pid = get_pid_from_cmd(cmd);
              if(pid != RTX_ERR) {
                // i think it is unnecessary to reset the sender/receiver pid on the message
                k_send_message(pid, receive_message); //send message to process associated with the command
              }
            }
          }
        }
    }
}
