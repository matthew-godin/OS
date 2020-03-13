/**
 * @file:   k_memory.c
 * @brief:  kernel memory managment routines
 * @author: Yiqing Huang
 * @date:   2014/01/17
 */

#include "k_memory.h"
#include "k_process.h"
#include "k_memory_queue.h"
#include "k_process_priority_queue.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

/* ----- Global Variables ----- */
U32 *gp_stack; /* The last allocated stack low address. 8 bytes aligned */
               /* The first stack starts at the RAM high address */
	       /* stack grows down. Fully decremental stack */
		   // global pointer stack
MEMORY_QUEUE gp_memory_queue;
extern PCB* gp_current_process;

/**
 * @brief: Initialize RAM as follows:

0x10008000+---------------------------+ High Address
          |    Proc 1 STACK           |
          |---------------------------|
          |    Proc 2 STACK           |
          |---------------------------|<--- gp_stack
          |                           |
          |        HEAP               |
          |                           |
          |---------------------------|
          |        PCB 2              |
          |---------------------------|
          |        PCB 1              |
          |---------------------------|
          |        PCB pointers       |
          |---------------------------|<--- gp_pcbs 0x39c
          |        Padding            |
          |---------------------------|  
          |Image$$RW_IRAM1$$ZI$$Limit |
          |...........................|          
          |       RTX  Image          |
          |                           |
0x10000000+---------------------------+ Low Address

*/

void memory_init(void)
{
	U8 *p_end = (U8 *)&Image$$RW_IRAM1$$ZI$$Limit; // bottom of available memory
	int i;
  
	/* these are all for the heap */
	U32* bottom_bound; // bottom of the heap
	int iter;
	
	/* 4 bytes padding */
	p_end += 4;

	/* allocate memory for pcb pointers   */
	gp_pcbs = (PCB **)p_end;
	p_end += (NUM_KERNEL_PROCS + NUM_TEST_PROCS) * sizeof(PCB *);
  
	for ( i = 0; i < NUM_KERNEL_PROCS + NUM_TEST_PROCS; i++ ) {
		gp_pcbs[i] = (PCB *)p_end;
		p_end += sizeof(PCB); 
	}
#ifdef DEBUG_0  
	printf("gp_pcbs[0] = 0x%x \r\n", gp_pcbs[0]);
	printf("gp_pcbs[1] = 0x%x \r\n", gp_pcbs[1]);
#endif
	
	/* prepare for alloc_stack() to allocate memory for stacks */
	
	gp_stack = (U32 *)RAM_END_ADDR;
	if ((U32)gp_stack & 0x04) { /* 8 bytes alignment */
		--gp_stack; 
	}
  
	// init heap	
	bottom_bound = (U32*)p_end;
	gp_memory_queue = init_mq(gp_memory_queue);
	for(iter = 0; iter < NUM_MEMORY_BLOCKS; ++iter) {
		U32* addr = (U32*)((U32)bottom_bound + iter*MEM_BLOCK_SIZE);
		gp_memory_queue = push_mq(gp_memory_queue, addr);
	}
}

/**
 * @brief: allocate stack for a process, align to 8 bytes boundary
 * @param: size, stack size in bytes
 * @return: The top of the stack (i.e. high address)
 * POST:  gp_stack is updated.
 */

U32 *alloc_stack(U32 size_b) 
{
	U32 *sp;
	sp = gp_stack; /* gp_stack is always 8 bytes aligned */
	
	/* update gp_stack */
	gp_stack = (U32 *)((U8 *)sp - size_b);
	
	/* 8 bytes alignement adjustment to exception stack frame */
	if ((U32)gp_stack & 0x04) {
		--gp_stack; 
	}
	return sp;
}

void *k_request_memory_block(void) {
	U32* returnAddr;
#ifdef DEBUG_0 
	printf("k_request_memory_block: entering...\n");
#endif /* ! DEBUG_0 */
	returnAddr = top_mq(gp_memory_queue);
	gp_memory_queue = pop_mq(gp_memory_queue);
	
	while (returnAddr == NULL) { //This could be an if as long as we return and there is memory available to us
		k_release_blocked_processor(WAITING_MEMORY); // call release processor to give up function
		returnAddr = top_mq(gp_memory_queue);
		gp_memory_queue = pop_mq(gp_memory_queue);
	}

	return returnAddr;
}

void* k_request_memory_block_non_blocking(void) {
		U32* returnAddr;
#ifdef DEBUG_0 
	printf("k_request_memory_block_non_blocking: entering...\n");
#endif /* ! DEBUG_0 */
	returnAddr = top_mq(gp_memory_queue);
	gp_memory_queue = pop_mq(gp_memory_queue);
	return returnAddr;
}

int k_release_memory_block(void *p_mem_blk) {
	PCB* unblocked_pcb ;
#ifdef DEBUG_0 
	printf("k_release_memory_block: releasing block @ 0x%x\r\n", p_mem_blk);
#endif /* ! DEBUG_0 */
	gp_memory_queue = push_mq(gp_memory_queue, (U32*)p_mem_blk);
	
	if(!pcb_waiting_memory_queue_is_empty()) { //if something in waiting, then unblock on waiting queue
		unblocked_pcb = pop_pcb_waiting_memory_queue(); //pop pcb from waiting queue;
		unblocked_pcb->m_state = RDY; //set unblocked pcb state to ready
		push_pcb_queue(unblocked_pcb); //push pcb to priority queue
		k_release_processor(); //gives up processor to potentially higher priority which just became unblocked, this is also preemption
	}
	return 1; // TODO: not sure what to return 
}
