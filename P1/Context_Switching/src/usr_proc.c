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
#include "common.h"
#include "wall_proc.h"

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

/* initialization table item */
PROC_INIT g_test_procs[NUM_TEST_PROCS];
int lastRunProc;
int totalTests = 6;
int passedTests = 0;
char str[10];

void set_test_procs() {
	int i;
	for( i = 0; i < NUM_TEST_PROCS; i++ ) { // User Procs
		g_test_procs[i].m_pid=(U32)(i+1);
		g_test_procs[i].m_priority=LOWEST;
		g_test_procs[i].m_stack_size=0x100;
	}

	// P1 tests
	// g_test_procs[0].m_priority = HIGH;
	// g_test_procs[0].mpf_start_pc = &proc1;
	// g_test_procs[1].mpf_start_pc = &proc2;
	// g_test_procs[2].mpf_start_pc = &proc3;
	 //g_test_procs[3].mpf_start_pc = &proc4;
	// g_test_procs[4].mpf_start_pc = &proc5;
	// g_test_procs[5].mpf_start_pc = &proc6;

	// Messaging Test
	g_test_procs[0].m_priority = HIGH;
	
	g_test_procs[0].mpf_start_pc = &proc1Message;
	g_test_procs[1].mpf_start_pc = &proc2Message;
	g_test_procs[2].mpf_start_pc = &proc3Message;
	g_test_procs[3].mpf_start_pc = &proc4Message;
	g_test_procs[4].mpf_start_pc = &proc5Message;
	g_test_procs[5].mpf_start_pc = &proc6Message;
}

/**
 * @brief: The null process, continuously calls release_processor()
 */
void proc1(void) {
	int priority;
	uart0_put_string("G024_test: START\r\n");
	uart0_put_string("G024_test: total 5 test cases\r\n");

	lastRunProc = 1;

	#ifdef DEBUG_0
	printf("Starting process 1\r\n");
	printf("Setting process 2 priority to 1 \r\n");
	#endif
	set_process_priority(2, 1);
	priority = get_process_priority(2);
	#ifdef DEBUG_0
	printf("Process 2 priority = %d \r\n", priority);
	#endif

	// Should jump to proc2
	#ifdef DEBUG_0
	printf("Setting process 1 (current process) to priority 2\r\n");
	#endif
	set_process_priority(1, 2);

	#ifdef DEBUG_0
	printf("Have returned to process 1\r\n");
	printf("Checking last process run was process 3\r\n");
	#endif
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

	#ifdef DEBUG_0
	printf("Starting process 2\r\n");
	printf("Checking last process run was process 1\r\n");
	#endif
	if (lastRunProc == 1) {
		uart0_put_string("G024_test: TEST 1 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 1 FAIL\r\n");
	}

	lastRunProc = 2;

	#ifdef DEBUG_0
	printf("Allocating %d memory blocks for process 2\r\n", NUM_MEMORY_BLOCKS);
	#endif
	for (i = 0; i < NUM_MEMORY_BLOCKS ; ++i) {
		mem_addresses[i] = request_memory_block();
		#ifdef DEBUG_0
		printf("Allocated block at: %d\r\n", mem_addresses[i]);
		#endif
	}

	//Should jump to proc 3
	set_process_priority(3, 0);
	lastRunProc = 2;
	#ifdef DEBUG_0
	printf("Have returned to process 2\r\n");
	#endif
	set_process_priority(4, 3);
	set_process_priority(3, 3);

	i = 0;
	// Expecting to not change processes
	#ifdef DEBUG_0
	printf("Releasing memory block from process 2: should not jump\r\n");
	printf("Checking last process run was process 1\r\n");
	#endif
	release_memory_block(mem_addresses[i++]);
	if (lastRunProc == 2) {
		uart0_put_string("G024_test: TEST 3 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 3 FAIL\r\n");
	}

	// Expecting to change processes
	#ifdef DEBUG_0
	printf("Switching to process 3 \r\n");
	#endif
	set_process_priority(3, 0);

	lastRunProc = 2;
	#ifdef DEBUG_0
	printf("Have returned to process 2\r\n");
	#endif
	set_process_priority(2, 1);
	#ifdef DEBUG_0
	printf("Releasing memory block from process 2: should jump to process 3\r\n");
	#endif
	release_memory_block(mem_addresses[i++]);

	#ifdef DEBUG_0
	printf("Have returned to process 2\r\n");
	printf("Checking last process run was process 1\r\n");
	#endif
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
		
	while(1) {
			release_processor();
}
	#ifdef DEBUG_0
	printf("Starting process 3\r\n");
	#endif
	lastRunProc = 3;

	// Updating priorities so we go to proc4 after we're blocked
	set_process_priority(1, 3);
	set_process_priority(2, 3);
	set_process_priority(4, 1);

	#ifdef DEBUG_0
	printf("Requesting for 1 block of memory (process should be blocked)\r\n");
	#endif
	mem_addr1 = request_memory_block();

	lastRunProc = 3;
	#ifdef DEBUG_0
	printf("Returned to process 3 \r\n");
	printf("Requesting for 1 block of memory (process should be blocked)\r\n");
	#endif
	mem_addr2 = request_memory_block();

	#ifdef DEBUG_0
	printf("Returned to process 3 \r\n");
	printf("Checking last process run was process 2 ");
	printf("since process 3 is no longer memory blocked and has higher priority than process 2\r\n");
	#endif
	if (lastRunProc == 2) {
		uart0_put_string("G024_test: TEST 4 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 4 FAIL\r\n");
	}
	lastRunProc = 3;

	#ifdef DEBUG_0
	printf("Updating all processes except process 3 to priority 3 \r\n");
	#endif
	set_process_priority(1, 3);
	set_process_priority(2, 3);
	set_process_priority(4, 3);
	set_process_priority(5, 3);
	set_process_priority(6, 3);
	#ifdef DEBUG_0
	printf("Releasing Processor 3: expect to stay in process 3 since it's higher priority than all other processes\r\n");
	#endif
	release_processor();
	#ifdef DEBUG_0
	printf("Updating process 3 to priority 3, expect to jump to 1\r\n");
	#endif
	set_process_priority(3, 3);

	#ifdef DEBUG_0
	printf("Have returned to process 3\r\n");
	printf("Checking last process run was process 6\r\n");
	#endif
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
		
	while(1) {
			release_processor();
}
	#ifdef DEBUG_0
	printf("Starting process 4\r\n");

	printf("Checking last process run was process 3 ");
	printf("since we should be memory blocked on process 3\r\n");
	#endif
	if (lastRunProc == 3) {
		uart0_put_string("G024_test: TEST 2 OK\r\n");
		passedTests++;
	} else {
		uart0_put_string("G024_test: TEST 2 FAIL\r\n");
	}
	// jump to proc 2
	lastRunProc = 4;
	set_process_priority(2, 0);

	#ifdef DEBUG_0
	printf("Have returned to process 4\r\n");
	printf("Checking last process run was process 2\r\n");
	#endif
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
	while(1) {
			release_processor();
}
		
	#ifdef DEBUG_0
	printf("Have returned to process 5\r\n");
	printf("Checking last process run was process 4\r\n");
	#endif
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
		while(1) {
			release_processor();
}
	#ifdef DEBUG_0
	printf("Have returned to process 6\r\n");
	printf("Checking last process run was process 5\r\n");
	#endif
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

void proc1Message(void) {
	MSG_BUF* message, *message2;
		
	uart0_put_string("G024_test: START\r\n");
	uart0_put_string("G024_test: total 6 tests\r\n");
	#ifdef DEBUG_0
	printf("Start of Proc1\r\n");
	#endif
	//---------------------------------------send message test
	set_process_priority(2,2);
	message = (MSG_BUF*) request_memory_block();
	message->mtext[0] = ':';
	message->mtext[1] = ')';
	message->mtext[2] = '\0';
	//*mem_addr1 = message;

	#ifdef DEBUG_0
	printf("PROC1: Sending a standard message from proc1 to proc2\r\n");
	#endif
	send_message(2, message);
	#ifdef DEBUG_0
	printf("PROC1: Going to Proc2");
	#endif
	set_process_priority(1, 2);
	
	#ifdef DEBUG_0
	printf("PROC1: Return to Proc1\r\n");
	#endif
	//just got back from proc 2
	//----------------------------------------delayed send test
	message = (MSG_BUF*) request_memory_block();
	message->mtext[0] = '2'; //should arrive second
	#ifdef DEBUG_0
	printf("PROC1: Sending first delayed message with text \'2\' \r\n");
	#endif
	delayed_send(2, message, 60);
	
	message2 = (MSG_BUF*) request_memory_block();
	message2->mtext[0] = '1'; //should arrive first 
	#ifdef DEBUG_0
	printf("PROC1: Sending second delayed message with text \'1\' \r\n");
	#endif
	delayed_send(2, message2, 30);


	#ifdef DEBUG_0
	printf("PROC1: Going to Proc2\r\n");
	#endif
	set_process_priority(1, 2); //downgrade its own priority, go to proc2message

	while (1) {
		release_processor();
	}
}

void proc2Message(void) {
	
	int* sender_id = NULL;
	MSG_BUF * receivedMessage, *receivedMessage2;
	
	//---------------------------------------------send message test
	#ifdef DEBUG_0
	printf("Start of Proc2\r\n");
	printf("PROC2: Receiving message\r\n");
	#endif
	receivedMessage = receive_message(sender_id);
	#ifdef DEBUG_0
	printf("PROC2: Received message\r\n");
	#endif
	if (receivedMessage->mtext[0] == ':' && receivedMessage->mtext[1] == ')') {
		uart0_put_string("G024_test: TEST 1 OK\r\n");
		passedTests += 1;
	} else {
		uart0_put_string("G024_test: TEST 1 FAIL\r\n");
	}
	release_memory_block(receivedMessage);
	
	//----------------------------------------------delayed send tests
	#ifdef DEBUG_0
	printf("PROC2: Going to Proc1\r\n");
	#endif
	set_process_priority(1,0); //go back to proc 1
	#ifdef DEBUG_0
	printf("PROC2: Receiving first delayed message \r\n");
	#endif
	receivedMessage = receive_message(sender_id);
	#ifdef debug_0
	printf("proc2: Received first delayed message \r\n");
	printf("proc2: Receiving second delayed message \r\n");
	#endif
	receivedMessage2 = receive_message(sender_id);
	#ifdef debug_0
	printf("proc2: Received second delayed message \r\n");
	#endif

	if(receivedMessage->mtext[0] == '1' && receivedMessage2->mtext[0] == '2') {
		uart0_put_string("G024_test: TEST 2 OK\r\n");
		passedTests += 1;
	} else {
		uart0_put_string("G024_test: TEST 2 FAIL\r\n");
	}
	
	release_memory_block(receivedMessage);
	release_memory_block(receivedMessage2);
	
	#ifdef debug_0
	printf("proc2: Going to Proc3\r\n");
	#endif
	set_process_priority(3,0); //make proc 3 higher priority, go there
	
	while (1) {
		release_processor();
	}
}


void proc3Message(void) {
	MSG_BUF * commandRegMsg, *commandRegMsg2, *receivedMsg;
	int i;
	char c;
	
	#ifdef DEBUG_0
	printf("Start of Proc3\r\n");
	#endif

	set_process_priority(4,1);
	set_process_priority(2,3);
	set_process_priority(1,3);

	
	#ifdef DEBUG_0
	printf("PROC3: Registering command %C\r\n");
	#endif
	commandRegMsg = (MSG_BUF*)request_memory_block();
	commandRegMsg->mtype = KCD_REG;
	commandRegMsg->mtext[0] = '%';
	commandRegMsg->mtext[1] = 'C';
	commandRegMsg->mtext[2] = '\0';
	send_message(PID_KCD, commandRegMsg);
	
	#ifdef DEBUG_0
	printf("PROC3: Registering command %D\r\n");
	#endif
	commandRegMsg2 = (MSG_BUF*)request_memory_block();
	commandRegMsg2->mtype = KCD_REG;
	commandRegMsg2->mtext[0] = '%';
	commandRegMsg2->mtext[1] = 'D';
	commandRegMsg2->mtext[2] = '\0';
	send_message(PID_KCD, commandRegMsg2);

	while(1) {
		
		receivedMsg = receive_message(NULL);
		
		if(receivedMsg->mtext[1] == 'C') { //print the params
			#ifdef DEBUG_0
			printf("PROC3: Received command %C");
			#endif
			i = 2;	
			c = receivedMsg->mtext[i];
	
			do {
				c = receivedMsg->mtext[i++];
				uart0_put_char(c);
			} while(c != '\0');
			uart0_put_string("G024_test: TEST 3A OK\r\n");
			passedTests += 1;

		} else if(receivedMsg->mtext[1] == 'D') { //print the params separated by a space!
			#ifdef DEBUG_0
			printf("PROC3: Received command %D");
			#endif
			i = 2;		
			
			do {
				c = receivedMsg->mtext[i++];
				uart0_put_char(c);
				uart0_put_char(' ');
			} while(c != '\0');	
				uart0_put_string("G024_test: TEST 3B OK\r\n");
				passedTests += 1;

		} else {
			//shouldn't get here lol
			uart0_put_string("G024_test: TEST 3 FAIL\r\n");
		}
		
		release_processor();
	}
}

void proc4Message(void) {
	MSG_BUF* command_invok;
	
	#ifdef DEBUG_0
	printf("Start of Proc4\r\n");
	#endif
	command_invok = request_memory_block();
	command_invok->mtext[0] = '%';
	command_invok->mtext[1] = 'D';
	command_invok->mtext[2] = '<';
	command_invok->mtext[3] = ':';
	command_invok->mtext[4] = 'D';
	command_invok->mtext[5] = '\0';
	#ifdef DEBUG_0
	printf("PROC4: Sending command %D\r\n");
	#endif
	if (send_message(PID_KCD, command_invok) == RTX_OK) {
		uart0_put_string("G024_test: TEST 4A OK\r\n");
		passedTests += 1;
	} else {
		uart0_put_string("G024_test: TEST 4A FAIL\r\n");
	}
	
	
	command_invok = request_memory_block();
	command_invok->mtext[0] = '%';
	command_invok->mtext[1] = 'C';
	command_invok->mtext[2] = '>';
	command_invok->mtext[3] = ':';
	command_invok->mtext[4] = 'C';
	command_invok->mtext[5] = '\0';
	#ifdef DEBUG_0
	printf("PROC4: Sending command %C\r\n");
	#endif
	if (send_message(PID_KCD, command_invok) == RTX_OK) {
		uart0_put_string("G024_test: TEST 4B OK\r\n");
		passedTests += 1;
	} else {
		uart0_put_string("G024_test: TEST 4B FAIL\r\n");
	}
	
	set_process_priority(5,0);
	
	while(1) {
		release_processor();
	}
}

void proc5Message(void) {
	MSG_BUF* msg_to_display;

	#ifdef DEBUG_0
	printf("Start of Proc5\r\n");
	#endif
	
	set_process_priority(3,3);
	set_process_priority(4,3);
	
	msg_to_display = (MSG_BUF*) request_memory_block();
	msg_to_display->mtype = CRT_DISPLAY;
	msg_to_display->mtext[0] = 'w';
	msg_to_display->mtext[1] = 'o';
	msg_to_display->mtext[2] = 'o';
	msg_to_display->mtext[3] = 'h';
	msg_to_display->mtext[4] = 'o';
	msg_to_display->mtext[5] = 'o';
	msg_to_display->mtext[6] = '!';
	msg_to_display->mtext[7] = '\0';

	if (send_message(PID_CRT, msg_to_display) == RTX_OK) {
		uart0_put_string("G024_test: TEST 5 OK\r\n");
		passedTests += 1;
	} else {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	
	set_process_priority(6,0);
	
	while(1) {
		release_processor();
	}
}

void proc6Message(void) {
	MSG_BUF* clock_cmd;
	int i;

	#ifdef DEBUG_0
	printf("Start of Proc6\r\n");
	#endif

	//deliberately set everything to have priority 0 so it loops through every process which just calls release_processor();
	//this is so we have more "time" to test the wall clock
	set_process_priority(1,0);
	set_process_priority(2,0);
	set_process_priority(3,0);
	set_process_priority(4,0);
	set_process_priority(5,0);

	
	clock_cmd = request_memory_block();
	clock_cmd->mtype = KCD_CMD;
	#ifdef DEBUG_0
	printf("PROC6: Resetting wall clock\r\n");
	#endif
	clock_cmd->mtext[0] = '%';
	clock_cmd->mtext[1] = 'W';
	clock_cmd->mtext[2] = 'R';
	clock_cmd->mtext[3] = '\0';
	send_message(PID_KCD, clock_cmd);
	
	for(i = 0; i < 10000; i++) {
		release_processor();
	}
	
	clock_cmd = request_memory_block();
	clock_cmd->mtype = KCD_CMD;
	#ifdef DEBUG_0
	printf("PROC6: Setting wall clock to 12:12:12\r\n");
	#endif
	clock_cmd->mtext[0] = '%';
	clock_cmd->mtext[1] = 'W';
	clock_cmd->mtext[2] = 'S';
	clock_cmd->mtext[3] = ' ';
	clock_cmd->mtext[4] = '1';
	clock_cmd->mtext[5] = '2';
	clock_cmd->mtext[6] = ':';
	clock_cmd->mtext[7] = '1';
	clock_cmd->mtext[8] = '2';
	clock_cmd->mtext[9] = ':';
	clock_cmd->mtext[10] = '1';
	clock_cmd->mtext[11] = '2';
	clock_cmd->mtext[12] = '\0';
	send_message(PID_KCD, clock_cmd);
	
	for(i = 0; i < 6000; i++) {
		release_processor();
	}
	
	clock_cmd = request_memory_block();
	clock_cmd->mtype = KCD_CMD;
	#ifdef DEBUG_0
	printf("PROC6: Resetting wall clock\r\n");
	#endif
	clock_cmd->mtext[0] = '%';
	clock_cmd->mtext[1] = 'W';
	clock_cmd->mtext[2] = 'R';
	clock_cmd->mtext[3] = '\0';
	send_message(PID_KCD, clock_cmd);
	
	for(i = 0; i < 5000; i++) {
		release_processor();
	}
	
	clock_cmd = request_memory_block();
	clock_cmd->mtype = KCD_CMD;
	#ifdef DEBUG_0
	printf("PROC6: Terminating wall clock\r\n");
	#endif
	clock_cmd->mtext[0] = '%';
	clock_cmd->mtext[1] = 'W';
	clock_cmd->mtext[2] = 'T';
	clock_cmd->mtext[3] = '\0';
	send_message(PID_KCD, clock_cmd);
	
	#ifdef DEBUG_0
	printf("PROC6: Sent wall clock commands (TEST 6)");
	#endif
	uart0_put_string("G024_test: TEST 6 OK\r\n");
	passedTests += 1;
	
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

	while(1) {
		#ifdef DEBUG_0
		printf("PROC6: Requesting Memory Block (Should eventually be blocked on memory)");
		#endif

		(request_memory_block() == NULL);
		release_processor();
	}
}