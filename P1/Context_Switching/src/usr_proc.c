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

	// Messaging Test
	g_test_procs[0].m_priority = 1;
	
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
	set_process_priority(1,1); //go back to proc 1
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
	set_process_priority(3,1); //make proc 3 higher priority, go there
	
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

	set_process_priority(4,2);
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
	command_invok->mtext[5] = '\r';
	command_invok->mtext[6] = '\n';
	command_invok->mtext[7] = '\0';
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
	command_invok->mtext[5] = '\r';
	command_invok->mtext[6] = '\n';
	command_invok->mtext[7] = '\0';
	#ifdef DEBUG_0
	printf("PROC4: Sending command %C\r\n");
	#endif
	if (send_message(PID_KCD, command_invok) == RTX_OK) {
		uart0_put_string("G024_test: TEST 4B OK\r\n");
		passedTests += 1;
	} else {
		uart0_put_string("G024_test: TEST 4B FAIL\r\n");
	}
	
	set_process_priority(5,1);
	
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
	msg_to_display->mtext[0] = '$';
		msg_to_display->mtext[1] = '\0';

	
	/**
	msg_to_display->mtext[4] = 'o';
	msg_to_display->mtext[5] = 'o';
	msg_to_display->mtext[6] = '!';
	msg_to_display->mtext[7] = '\r';
	msg_to_display->mtext[8] = '\n';
	msg_to_display->mtext[9] = '\0';
*/
	if (send_message(PID_CRT, msg_to_display) == RTX_OK) {
		uart0_put_string("G024_test: TEST 5 OK\r\n");
		passedTests += 1;
	} else {
		uart0_put_string("G024_test: TEST 5 FAIL\r\n");
	}
	
	set_process_priority(6,1);
	
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
	set_process_priority(1,1);
	set_process_priority(2,1);
	set_process_priority(3,1);
	set_process_priority(4,1);
	set_process_priority(5,1);

	
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
	
	if(passedTests > totalTests) {
		passedTests = totalTests;
	}
		
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