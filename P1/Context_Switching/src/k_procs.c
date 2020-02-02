#include "rtx.h"
#include "k_procs.h"

PROC_INIT g_kernel_procs[NUM_KERNEL_PROCS];

void set_kernel_procs() {
    g_kernel_procs[0].m_pid= 0;
    g_kernel_procs[0].m_priority= 4;
    g_kernel_procs[0].m_stack_size= 128;
    g_kernel_procs[0].mpf_start_pc= &null_proc;
}

void null_proc() {
    while(1) {
        release_processor();
    }
}
