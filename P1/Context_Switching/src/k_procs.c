#include "k_procs.h"
#include "common.h"
#include "k_timeout_queue.h"
#include "k_message.h"
#include "k_command_map.h"
#include "hot_key_procs.h"
#include "LPC17xx.h"
#include "uart.h"
#include "rtx.h"


PROC_INIT g_kernel_procs[NUM_KERNEL_PROCS];
extern TIMEOUT_QUEUE timeout_queue;
extern uint8_t *gp_buffer;

MSG_BUF* kcd_buffer = NULL;
int kcd_buffer_index = 0;

void set_kernel_procs() {
    g_kernel_procs[0].m_pid= 0;
    g_kernel_procs[0].m_priority= 4;
    g_kernel_procs[0].m_stack_size= 0x200;
    g_kernel_procs[0].mpf_start_pc= &null_proc;

    g_kernel_procs[1].m_pid= PID_KCD;
    g_kernel_procs[1].m_priority= 0;
    g_kernel_procs[1].m_stack_size= 0x200;
    g_kernel_procs[1].mpf_start_pc= &kcd_proc;

   g_kernel_procs[2].m_pid= PID_CRT;
   g_kernel_procs[2].m_priority= 0;
    g_kernel_procs[2].m_stack_size= 0x200;
    g_kernel_procs[2].mpf_start_pc= &crt_proc;
}

void null_proc() {
    while(1) {
        k_release_processor();
    }
}

void timer_i_proc() {
	  int pid = 0;
    MSG_BUF* ready_message = NULL;
    MSG_BUF* new_message;
    
	  __disable_irq();

		new_message = k_receive_message_non_blocking(NULL);

    while(new_message != NULL) {
        insert_tq_msg(&timeout_queue, new_message);
        new_message = k_receive_message_non_blocking(NULL);
    }

    if (!is_tq_empty(&timeout_queue)) {
        top_tq(&timeout_queue)->m_time -= 1;
    }
    while(!is_tq_empty(&timeout_queue) && top_tq(&timeout_queue)->m_time == 0) {
        ready_message = pop_tq_msg(&timeout_queue);
        pid = ready_message->m_recv_pid;
        i_send_message(pid, ready_message);
    }
    
		__enable_irq();
}

//called after each character
void uart_i_proc(char c) {
	 MSG_BUF* kcd_msg_env;
	 MSG_BUF* crt_msg_env;
	
	 __disable_irq();
	
   //display character regardless
   crt_msg_env = (MSG_BUF*) k_request_memory_block_non_blocking(); // non-blocking
	 if(crt_msg_env != NULL ){
		 crt_msg_env->mtype = CRT_DISPLAY;
		 crt_msg_env->mtext[0] = c; //add character to be printed in message body
		 i_send_message(PID_CRT, crt_msg_env);	 
	 }

   //handle hotkey
	 #ifdef _DEBUG_HOTKEYS
   if (c == '!') {
     print_ready_queue();
   } else if(c == '@') {
     print_blocked_memory_queue();
   } else if (c == '#') {
     print_blocked_message_queue();
   }
	 #endif

   //build message buffer for kcd display
   if(kcd_buffer == NULL) { //if buffer has been flushed, we need a new memory block
     kcd_buffer = (MSG_BUF*) k_request_memory_block_non_blocking(); // non-blocking
    
   }
	 
	 if(kcd_buffer != NULL) { //if there is in fact a memory block
		 kcd_buffer->mtype = KCD_CMD; //this doesnt need to happen every time, only after requesting mem block. but it needs to be null checked
		 kcd_buffer->mtext[kcd_buffer_index++] = c; //put char on buffer, increment index

		 if(c == '\r') { //last character TODO
			 kcd_msg_env = kcd_buffer; // set kcd_msg_evn to buffer
			 kcd_buffer = NULL;  //flush the buffer
			 kcd_buffer_index = 0; //reset index

			 i_send_message(PID_KCD, kcd_msg_env); //send message to kcd
		 }
	 }
	 
	 __enable_irq();
}

void crt_proc() {
    MSG_BUF* receive_msg = NULL;
    char* received_char;
	  LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;
	
	
    while(1) {
        receive_msg = (MSG_BUF*) receive_message(NULL);
        //send it to UART1_IRQ
				pUart->IER ^= IER_THRE;
			  gp_buffer =(uint8_t*) received_char;
        release_memory_block(receive_msg);
				release_processor();
    }
}

//hot keys will not enter this code
//hot keys should be handled in uart i process
// this assumes that the message received ty the kcd is a COMLETE COMMAND,
// not just individual characters
void kcd_proc() {
    MSG_BUF* receive_msg = NULL;
    char cmd;
    int pid;
		
    while(1) {
        receive_msg = (MSG_BUF*) receive_message(NULL);
        if(receive_msg != NULL) {
					cmd = receive_msg->mtext[1];

          if(receive_msg->mtype == KCD_REG) { //command registration
            if(receive_msg->mtext[0] =='%') { //check if valid keyboard command
              insert_cmd(cmd, receive_msg->m_send_pid);
            }
          } else { // invocation of command

            if(receive_msg->mtext[0] =='%') {  //check if valid keyboard command
              pid = get_pid_from_cmd(cmd);
              if(pid != RTX_ERR) {
                // i think it is unnecessary to reset the sender/receiver pid on the message
                send_message(pid, receive_msg); //send message to process associated with the command
              }
            }
          }
        }
				release_processor();
    }
}
