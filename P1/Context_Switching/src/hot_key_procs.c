#include "hot_key_procs.h"
#include "common.h"
#include "uart_polling.h"


extern PCB* gp_pcb_queue[NUM_KERNEL_PROCS + NUM_TEST_PROCS + 1];
extern PCB* gp_pcb_waiting_memory_queue[NUM_KERNEL_PROCS + NUM_TEST_PROCS + 1];
extern PCB* gp_pcb_message_waiting_queue[];

void print_ready_queue() {
  print_queue(gp_pcb_queue, NUM_KERNEL_PROCS + NUM_TEST_PROCS + 1);
}

void print_blocked_memory_queue() {
  print_queue(gp_pcb_waiting_memory_queue, NUM_KERNEL_PROCS + NUM_TEST_PROCS + 1);
}

void print_blocked_message_queue() {
  print_queue(gp_pcb_message_waiting_queue, 16);
}

void print_queue(PCB* queue[], int size) {
  int i, pid, priority, a ,b;
	 uart0_put_string("\r\n"); //newline for cleanliness
  for(i = 0; i < size; i++) {
    if(queue[i] != NULL) {
      pid = queue[i]->m_pid;
      priority = queue[i]->m_priority;
			
			a = pid/10;
			b = pid%10;
      
			uart0_put_string("Pid: ");
			if(a != 0) uart0_put_char(a + '0');
			uart0_put_char(b + '0');
			uart0_put_string(", priority: ");
			uart0_put_char(priority + '0');
      uart0_put_string("\r\n");
    }
  }
}
