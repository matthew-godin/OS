
#include "wall_proc.h"
#include "common.h"
#include "rtx.h"

char wall_time[11];
int wall_is_running = 0; //wall is initially not running
int hour = 0, min = 0, sec = 0;
PROC_INIT g_wall_proc[1];


void set_wall_proc() {
	//wall proc
	g_wall_proc[0].m_pid= (U32) 11;
	g_wall_proc[0].m_priority= 0;
	g_wall_proc[0].m_stack_size=0x100;
	g_wall_proc[0].mpf_start_pc= &wall_proc;
}

void wall_proc() {
  MSG_BUF* receive_msg = NULL;
  while(1) {
    receive_msg = receive_message(NULL);
		//update wall time always calls release_memory_block
    update_wall_time(receive_msg);
		
		release_processor();
  }
}

//reset, terminate, or set to specific time, this is called ONLY by the wall_proc
//note, this assumes a correctly formatted wall proc command
void update_wall_time(MSG_BUF* msg) {
  MSG_BUF* crt_msg_env;
  char* cmd_str = msg->mtext;
  int i;
  if(cmd_str[2] == 'R') { //reset
    wall_is_running = 1;
    reset_wall_time();
  } else if(cmd_str[2] == 'T') { //terminate
    wall_is_running = 0;
  } else if(cmd_str[2] == 'S') { //set to a specific time
		//just copies the string over, no check yet
		wall_is_running = 1;
		
		sec = ((int)  (cmd_str[4+7]-'0') ) + (10* ((int) (cmd_str[4+6]-'0') ) );
		min = ((int) (cmd_str[4+4]-'0') ) + (10* ((int) (cmd_str[4+3]-'0') ) );
		hour = ((int) (cmd_str[4+1]-'0') ) + (10* ((int) (cmd_str[4+0]-'0') ) );
		
		set_wall_str();
  }
	release_memory_block(msg);

  if(wall_is_running) {
    crt_msg_env = (MSG_BUF*) request_memory_block();
    crt_msg_env->mtype = CRT_DISPLAY;

    for(i = 0; i < 11; i++) {
      crt_msg_env->mtext[i] = wall_time[i];
    }
    send_message(PID_CRT, crt_msg_env);
  } 
}

//should be called every second by timer
void increment_wall_time() {
	MSG_BUF* crt_msg_env;
	int i;
	if(wall_is_running) {

		sec++;
		if(sec == 60) {
			sec = 0;
			min++;
		}
		if(min == 60) {
			min = 0;
			hour++;
		}

		set_wall_str();
    crt_msg_env = (MSG_BUF*) k_request_memory_block();
    crt_msg_env->mtype = CRT_DISPLAY;

    for(i = 0; i < 11; i++) {
      crt_msg_env->mtext[i] = wall_time[i];
    }
    i_send_message(PID_CRT, crt_msg_env);
  }
}

void reset_wall_time() {
		hour = 0;
		min = 0;
		sec = 0;
	  set_wall_str();
}

void set_wall_str() {
		wall_time[0] = hour/10 + '0';
		wall_time[1] = hour%10 + '0';
	  wall_time[2] = ':';
		wall_time[3] = min/10 + '0';
		wall_time[4] = min%10 + '0';
	  wall_time[5] = ':';
		wall_time[6] = sec/10 + '0';
		wall_time[7] = sec%10 + '0';
	  wall_time[8] = '\n';
		wall_time[9] = '\r';
		wall_time[10] = '\0';
}
