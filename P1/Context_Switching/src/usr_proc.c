/**
 * @file:   usr_proc.c
 * @brief:  Two user processes: proc1 and proc2
 * @author: Yiqing Huang
 * @date:   2014/01/17
 * NOTE: Each process is in an infinite loop. Processes never terminate.
 */

#include "rtx.h"
#include "uart_polling.h"
#include "usr_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TOTAL_PROCS; i++ ) { // User Procs
		g_test_procs[i].m_pid=(U32)(i+NUM_KERNEL_PROCS);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}
	g_test_procs[0].mpf_start_pc = &proc1;
	g_test_procs[1].mpf_start_pc = &proc2;
	g_test_procs[2].mpf_start_pc = &proc3;
	g_test_procs[3].mpf_start_pc = &proc4;
	g_test_procs[4].mpf_start_pc = &proc5;
	g_test_procs[5].mpf_start_pc = &proc6;
}

/**
 * @brief: The null process, continuously calls release_processor()
 */
void proc1(void) {
	int i=0;
	U32* wowe[7];

	printf("proc1 \n");
	set_process_priority(1, 2);
	while (i < 7) {
		wowe[i] = request_memory_block();
		printf("mem1: %d \n", wowe[i]);
		++i;
	}
	set_process_priority(2, 1);
	release_memory_block(wowe[0]);
	printf("released:\n");
	while (1) {
		release_processor();
	}
}

/**
 * @brief: a process that prints five uppercase letters
 *         and then yields the cpu.
 */
void proc2(void)
{
	int i;
	U32* wowe[2];

	printf("proc2 \n");
	for (i =0; i < 2; ++i) {
		wowe[i] = request_memory_block();
		printf("mem2: %d \n", wowe[i]);
	}
	printf("proc2 back babeeey\n");
	while (1) {
		release_processor();
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc3(void)
{
	int sheme3;
	printf("proc3 \n");
	sheme3 = get_process_priority(3);
	printf("proc3 > %d \n", sheme3);
	set_process_priority(2, 1);
	sheme3 = get_process_priority(3);
	while (1) {
		release_processor();
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc4(void)
{
	while (1) {
		release_processor();
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc5(void)
{
	while (1) {
		release_processor();
	}
}

/**
 * @brief: a process that prints five numbers
 *         and then yields the cpu.
 */
void proc6(void)
{
	int i = 0;
	int ret_val = 20;
	while (1) {
		release_processor();
	}
	while ( 1) {
		if ( i != 0 && i%5 == 0 ) {
			uart0_put_string("\n\r");
			ret_val = release_processor();
#ifdef DEBUG_0
			printf("proc5: ret_val=%d\n", ret_val);
#endif /* DEBUG_0 */
		}
		uart0_put_char('0' + i%10);
		i++;
	}
}
