/**
 * @file:   k_process.c  
 * @brief:  process management C file
 * @author: Yiqing Huang
 * @author: Thomas Reidemeister
 * @date:   2014/01/17
 * NOTE: The example code shows one way of implementing context switching.
 *       The code only has minimal sanity check. There is no stack overflow check.
 *       The implementation assumes only two simple user processes and NO HARDWARE INTERRUPTS. 
 *       The purpose is to show how context switch could be done under stated assumptions. 
 *       These assumptions are not true in the required RTX Project!!!
 *       If you decide to use this piece of code, you need to understand the assumptions and
 *       the limitations. 
 */

#include <LPC17xx.h>
#include <system_LPC17xx.h>
#include "uart_polling.h"
#include "k_process_priority_queue.h"
#include "k_process.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* ----- Global Variables ----- */
PCB **gp_pcbs;                  /* array of pcbs */
PCB *gp_current_process = NULL; /* always point to the current RUN process */
extern PCB* gp_pcb_queue[NUM_TOTAL_PROCS];

/* process initialization table */
PROC_INIT g_proc_table[NUM_TOTAL_PROCS];
extern PROC_INIT g_test_procs[NUM_TEST_PROCS];
extern PROC_INIT g_kernel_procs[NUM_KERNEL_PROCS];

/**
 * @biref: initialize all processes in the system
 * NOTE: We assume there are only two user processes in the system in this example.
 */
void process_init() 
{
	int i;
	int j;
	U32 *sp;
	PCB* temp;
  
        /* fill out the initialization table */
	set_test_procs();
	set_kernel_procs();
	for ( i = 0; i < NUM_TEST_PROCS; i++ ) {
		g_proc_table[i].m_pid = g_test_procs[i].m_pid;
		g_proc_table[i].m_priority = g_test_procs[i].m_priority;
		g_proc_table[i].m_stack_size = g_test_procs[i].m_stack_size;
		g_proc_table[i].mpf_start_pc = g_test_procs[i].mpf_start_pc;
	}
	for ( i = 0; i < NUM_KERNEL_PROCS; i++ ) {
		j = NUM_TEST_PROCS + i;
		g_proc_table[j].m_pid = g_kernel_procs[i].m_pid;
		g_proc_table[j].m_priority = g_kernel_procs[i].m_priority;
		g_proc_table[j].m_stack_size = g_kernel_procs[i].m_stack_size;
		g_proc_table[j].mpf_start_pc = g_kernel_procs[i].mpf_start_pc;
	}
  
	/* initilize exception stack frame (i.e. initial context) for each process */
	for ( i = 0; i < NUM_TOTAL_PROCS; i++ ) {
		int j;
		(gp_pcbs[i])->m_pid = (g_proc_table[i]).m_pid;
		(gp_pcbs[i])->m_priority = (g_proc_table[i]).m_priority;
		(gp_pcbs[i])->m_state = NEW;
		
		sp = alloc_stack((g_proc_table[i]).m_stack_size);
		*(--sp)  = INITIAL_xPSR;      // user process initial xPSR  
		*(--sp)  = (U32)((g_proc_table[i]).mpf_start_pc); // PC contains the entry point of the process
		for ( j = 0; j < 6; j++ ) { // R0-R3, R12 are cleared with 0
			*(--sp) = 0x0;
		}
		(gp_pcbs[i])->mp_sp = sp;
	}
	
	for (i = 0; i < NUM_TOTAL_PROCS; ++i) {
		temp = gp_pcbs[i];
	}

	init_pcb_queue();
	for(i=0; i < NUM_TOTAL_PROCS; ++i) {
		push_pcb_queue(gp_pcbs[i]); // add all pcbs to process priority queue
	}
}

/*@brief: scheduler, pick the pid of the next to run process
 *@return: PCB pointer of the next to run process
 *         NULL if error happens
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */

PCB *scheduler(void)
{
	PCB* next_pcb;
	
	if(gp_current_process != NULL) {
		gp_current_process->m_state = RDY; // sets state of old process to ready before pushing to pcb_queue
		push_pcb_queue(gp_current_process);
	}
	next_pcb = pop_pcb_queue();
	
	return next_pcb;
}

/*@brief: switch out old pcb (p_pcb_old), run the new pcb (gp_current_process)
 *@param: p_pcb_old, the old pcb that was in RUN
 *@return: RTX_OK upon success
 *         RTX_ERR upon failure
 *PRE:  p_pcb_old and gp_current_process are pointing to valid PCBs.
 *POST: if gp_current_process was NULL, then it gets set to pcbs[0].
 *      No other effect on other global variables.
 */
int process_switch(PCB *p_pcb_old) 
{
	PROC_STATE_E state;
	
	state = gp_current_process->m_state;

	if (state == NEW) {
		if (gp_current_process != p_pcb_old && p_pcb_old->m_state != NEW) {
			p_pcb_old->m_state = RDY;
			p_pcb_old->mp_sp = (U32 *) __get_MSP();
		}
		gp_current_process->m_state = RUN;
		__set_MSP((U32) gp_current_process->mp_sp);
		__rte();  // pop exception stack frame from the stack for a new processes
	} 
	
	/* The following will only execute if the if block above is FALSE */

	if (gp_current_process != p_pcb_old) {
		if (state == RDY){ 		
			p_pcb_old->m_state = RDY; 
			p_pcb_old->mp_sp = (U32 *) __get_MSP(); // save the old process's sp
			gp_current_process->m_state = RUN;
			__set_MSP((U32) gp_current_process->mp_sp); //switch to the new proc's stack    
		} else {
			gp_current_process = p_pcb_old; // revert back to the old proc on error
			return RTX_ERR;
		} 
	}
	return RTX_OK;
}
/**
 * @brief release_processor(). 
 * @return RTX_ERR on error and zero on success
 * POST: gp_current_process gets updated to next to run process
 */
int k_release_processor(void)
{
	PCB *p_pcb_old = NULL;
	
	p_pcb_old = gp_current_process;
	gp_current_process = scheduler(); // sets gp_current_process to newly selected process based on priority
	
	if ( gp_current_process == NULL  ) { // may never hit this condition
		gp_current_process = p_pcb_old; // revert back to the old process
		return RTX_ERR;
	}
    if ( p_pcb_old == NULL ) {
		p_pcb_old = gp_current_process;
	}
	process_switch(p_pcb_old);
	return RTX_OK;
}

int set_process_priority(int process_id, int priority) {
	int i;
	//TODO: a process cannot change the priority of the i-process
	if (process_id == 0 && priority != 4) {
		return 0;
	}
	if(process_id != 0 && priority ==4) {
		return 0;
	}
	for ( i = 0; i < NUM_TOTAL_PROCS; i++ ) {
		if((gp_pcbs[i])->m_pid == process_id) { //iterate through pcb array and search for matching PID
			(gp_pcbs[i])->m_priority = priority;
			updated_pcb_priority(gp_pcbs[i]->m_pid);
			
			if(process_id != gp_current_process->m_pid && /* if processor whose priority is  changed isn't the current processor */
				 priority < gp_current_process->m_priority && /*if processor whose priority is changed has a more important priority (lower number)*/
			   ((gp_pcbs[i])->m_state == RDY || (gp_pcbs[i])->m_state == NEW)) { //if processor whose priority is changed is in ready or new state
					k_release_processor(); //TODO: not sure if we should just be release_processor()
			}
			return 1;
		}
	}
	return 0;
}

int get_process_priority(int process_id) {
	int i;
	for ( i = 0; i < NUM_TOTAL_PROCS; i++ ) {
		if((gp_pcbs[i])->m_pid == process_id) {  //iterate through pcb array and search for matching PID
			return (gp_pcbs[i])->m_priority;
		}
	}
	return -1;
}
