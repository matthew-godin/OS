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
int lastRunProc;
int totalTests = 5;
int passedTests = 0;
char str[10];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TOTAL_PROCS; i++ ) { // User Procs
		g_test_procs[i].m_pid=(U32)(i+NUM_KERNEL_PROCS);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}
	g_test_procs[0].m_priority = HIGH;
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
	int priority;
	uart0_put_string("G024_test: START\r\n");
	uart0_put_string("G024_test: total 5 test cases\r\n");

	lastRunProc = 1;

	printf("Starting process 1\r\n");
	printf("Setting process 2 priority to 1: \r\n");
	set_process_priority(2, 1);
	priority = get_process_priority(2);
	printf("Process 2 priority = %d \r\n", priority);

	// Should jump to proc2
	printf("Setting proc1 (running) to priority 2\r\n");
	set_process_priority(1, 2);

	printf("Have returned to process 1\r\n");
	printf("Checking last process run was process 3\r\n");
	if (lastRunProc != 3) {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	lastRunProc = 1;

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
	U32* mem_addresses[NUM_MEMORY_BLOCKS];
	int i;

	printf("Starting process 2\r\n");
	printf("Checking last process run was process 1\r\n");
	if (lastRunProc == 1) {
		uart0_put_string("G024_test: TEST 1 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 1 FAIL\r\n");
	}

	lastRunProc = 2;

	printf("Allocating %d memory blocks for process 2\r\n", NUM_MEMORY_BLOCKS);
	for (i = 0; i < NUM_MEMORY_BLOCKS ; ++i) {
		mem_addresses[i] = request_memory_block();
		printf("Allocated block at: %d\r\n", mem_addresses[i]);
	}

	//Should jump to proc 3
	set_process_priority(3, 0);
	lastRunProc = 2;
	printf("Have returned to process 2\r\n");
	set_process_priority(4, 3);
	set_process_priority(3, 3);

	i = 0;
	// Expecting to not change processes
	printf("Releasing memory block from process 2: should not jump\r\n");
	printf("Checking last process run was process 1\r\n");
	release_memory_block(mem_addresses[i++]);
	if (lastRunProc == 2) {
		uart0_put_string("G024_test: TEST 3 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 3 FAIL\r\n");
	}

	// Expecting to change processes
	printf("Switching to process 3 \r\n");
	set_process_priority(3, 0);

	lastRunProc = 2;
	printf("Have returned to process 2\r\n");
	set_process_priority(2, 1);
	printf("Releasing memory block from process 2: should jump to process 3\r\n");
	release_memory_block(mem_addresses[i++]);

	printf("Have returned to process 2\r\n");
	printf("Checking last process run was process 1\r\n");
	if (lastRunProc != 1) {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	lastRunProc = 2;

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
	U32* mem_addr1;
	U32* mem_addr2;
	printf("Starting process 3\r\n");
	lastRunProc = 3;

	// Updating priorities so we go to proc4 after we're blocked
	set_process_priority(1, 3);
	set_process_priority(2, 3);
	set_process_priority(4, 1);

	printf("Requesting for 1 block of memory (process should be blocked)\r\n");
	mem_addr1 = request_memory_block();

	lastRunProc = 3;
	printf("Returned to process 3 \r\n");
	printf("Requesting for 1 block of memory (process should be blocked)\r\n");
	mem_addr2 = request_memory_block();

	printf("Returned to process 3 \r\n");
	printf("Checking last process run was process 2 ");
	printf("since process 3 is no longer memory blocked and has higher priority than process 2\r\n");
	if (lastRunProc == 2) {
		uart0_put_string("G024_test: TEST 4 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 4 FAIL\r\n");
	}
	lastRunProc = 3;

	printf("Updating all processes except process 3 to priority 3 \r\n");
	set_process_priority(1, 3);
	set_process_priority(2, 3);
	set_process_priority(4, 3);
	set_process_priority(5, 3);
	set_process_priority(6, 3);
	printf("Releasing Processor 3: expect to stay in process 3 since it's higher priority than all other processes\r\n");
	release_processor();
	printf("Updating process 3 to priority 3, expect to jump to 1\r\n");
	set_process_priority(3, 3);

	printf("Have returned to process 3\r\n");
	printf("Checking last process run was process 6\r\n");
	if (lastRunProc != 6) {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	lastRunProc = 3;

	uart0_put_string("G024_test: ");
	sprintf(str, "%d", passedTests);
	uart0_put_string(str);
	uart0_put_char('/');
	sprintf(str, "%d", totalTests);
	uart0_put_string(str);
	uart0_put_string(" tests OK \r\n");

	uart0_put_string("G024_test: ");
	sprintf(str, "%d", totalTests- passedTests);
	uart0_put_string(str);
	uart0_put_char('/');
	sprintf(str, "%d", totalTests);
	uart0_put_string(str);
	uart0_put_string(" tests FAIL \r\n");

	uart0_put_string("G024_test: END \r\n");


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
	printf("Starting process 4\r\n");

	printf("Checking last process run was process 3 ");
	printf("since we should be memory blocked on process 3\r\n");
	if (lastRunProc == 3) {
		uart0_put_string("G024_test: TEST 2 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 2 FAIL\r\n");
	}
	// jump to proc 2
	lastRunProc = 4;
	set_process_priority(2, 0);

	printf("Have returned to process 4\r\n");
	printf("Checking last process run was process 2\r\n");
	if (lastRunProc != 2) {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	lastRunProc = 4;

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
	printf("Have returned to process 5\r\n");
	printf("Checking last process run was process 4\r\n");
	if (lastRunProc != 4) {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	lastRunProc = 5;

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
	printf("Have returned to process 6\r\n");
	printf("Checking last process run was process 5\r\n");
	if (lastRunProc == 5) {
		uart0_put_string("G024_test: TEST 5 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	lastRunProc = 6;

	while (1) {
		release_processor();
	}
}
