/* @brief: common defines and structs for both kernel and user
 * @file: common.h
 * @author: Yiqing Huang
 * @date: 2017/01/04
 */

#ifndef COMMON_H_
#define COMMON_H_

/* Definitions */

#define BOOL unsigned char

#define TRUE 1
#define FALSE 0
#define NULL 0
#define RTX_ERR -1
#define RTX_OK 0

/* Process IDs */
#define PID_NULL 0
#define PID_P1   1
#define PID_P2   2
#define PID_P3   3
#define PID_P4   4
#define PID_P5   5
#define PID_P6   6
#define PID_A    7
#define PID_B    8
#define PID_C    9
#define PID_SET_PRIO     10
#define PID_CLOCK        11
#define PID_KCD          12
#define PID_CRT          13
#define PID_TIMER_IPROC  14
#define PID_UART_IPROC   15


/* Process Priority. The bigger the number is, the lower the priority is*/
#define HIGH    0
#define MEDIUM  1
#define LOW     2
#define LOWEST  3

/* Message Types */
#define DEFAULT 0
#define KCD_REG 1
#define KCD_CMD 2
#define CRT_DISPLAY 3


/* PID for useful processes */
#define PID_CRT 13
#define PID_KCD 12

#define NUM_TEST_PROCS 7
#define NUM_KERNEL_PROCS 3

/* ----- Types ----- */
typedef unsigned char U8;
typedef unsigned int U32;

/* common data structures in both kernel and user spaces */

/* initialization table item */
typedef struct proc_init
{	
	int m_pid;	        /* process id */ 
	int m_priority;         /* initial priority, not used in this example. */ 
	int m_stack_size;       /* size of stack in words */
	void (*mpf_start_pc) ();/* entry point of the process */    
} PROC_INIT;

/* message buffer */
typedef struct msgbuf
{
#ifdef K_MSG_ENV
	void *mp_next;		/* ptr to next message received*/
	int m_send_pid;		/* sender pid */
	int m_recv_pid;		/* receiver pid */
	int m_kdata[5];		/* extra 20B kernel data place holder */
	int m_time;             /* time relative to previous message in timeout_queue */
#endif
	int mtype;              /* user defined message type */
	char mtext[24];          /* body of the message */
} MSG_BUF;

#endif // COMMON_H_
