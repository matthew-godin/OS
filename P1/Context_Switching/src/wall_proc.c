
#include "wall_proc.h"
#include "common.h"
#include "rtx.h"

char* base_wall_time = "00:00:00";
char wall_time[9];
int wall_is_running = 0; //wall is initially not running
int hour = 0, min = 0, sec = 0;

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
    for(i= 0; i < 8; i++) {
      wall_time[i] = cmd_str[i+4];
    }
  }
	release_memory_block(msg);

  if(wall_is_running) {
    crt_msg_env = (MSG_BUF*) request_memory_block();
    crt_msg_env->mtype = CRT_DISPLAY;

    for(i = 0; i < 8; i++) {
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
		wall_time[0] = hour/10 + '0';
		wall_time[1] = hour%10 + '0';
		wall_time[3] = min/10 + '0';
		wall_time[4] = min%10 + '0';
		wall_time[6] = sec/10 + '0';
		wall_time[7] = sec%10 + '0';
	
    crt_msg_env = (MSG_BUF*) k_request_memory_block();
    crt_msg_env->mtype = CRT_DISPLAY;

    for(i = 0; i < 8; i++) {
      crt_msg_env->mtext[i] = wall_time[i];
    }
    i_send_message(PID_CRT, crt_msg_env);
  }
}

void reset_wall_time() {
		int i;
		for(i = 0; i < 9; i++) {
				wall_time[i] = base_wall_time[i];
		}
}
