/** 
 * @file:   k_rtx_init.c
 * @brief:  Kernel initialization C file
 * @auther: Yiqing Huang
 * @date:   2014/01/17
 */

#include "k_rtx_init.h"
#include "uart_polling.h"
#include "k_memory.h"
#include "k_process.h"
#include "k_message.h"
#include "timer.h"
#include "uart.h"
#include "wall_proc.h"
#include "k_command_map.h"

void k_rtx_init(void)
{
        __disable_irq();
				timer_init(0);
        uart0_init();
	uart1_init();
        memory_init();
        process_init();

        command_map_init();
        mailboxes_init();
        timeout_queue_init();
				reset_wall_time();
        uart0_irq_init();
        uart1_irq_init();
        __enable_irq();

	/* start the first process */

        k_release_processor();
}
