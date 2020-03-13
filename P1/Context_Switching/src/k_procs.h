#ifndef K_PROC_H_
#define K_PROC_H_

void set_kernel_procs(void);
void null_proc(void);
void timer_i_proc(void);
void uart_i_proc(char c);
void crt_proc(void);
void kcd_proc(void);

#endif
