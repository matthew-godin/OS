
#include "wall_proc.h"
#include "common.h"
#include "rtx.h"

//reset, terminate, or set to specific time, this is called ONLY by the wall_proc
//note, this assumes a correctly formatted wall proc command
void update_wall_time(MSG_BUF* msg) {
  MSG_BUF* crt_msg_env;
  char* cmd_str = msg->mtext;
  int i;
  if(cmd_str[2] == 'R') { //reset
    wall_is_running = 1;
    wall_time = "00:00:00";
  } else if(cmd_str[2] == 'T') { //terminate
    wall_is_running = 0;
  } else if(cmd_str[2] == 'S') { //set to a specific time
    for(i= 0; i < 8; i++) {
      wall_time[i] = cmd_str[i+4];
    }
  }

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
  char newChar;
  int i;

  newChar = helper_increment(wall_time[7]);
  if(newChar == '0') { //need to increment another digit
    newChar = helper_increment(wall_time[6]);
    if(newChar == '0') {
      newChar = helper_increment(wall_time[4]);
      if(newChar == '0') {
        newChar = helper_increment(wall_time[3]);
        if(newChar == '0') {
          newChar = helper_increment(wall_time[1]);
          if(newChar == '0') {
            newChar = helper_increment(wall_time[0]);
          }
        }
      }
    }
  }
  if(wall_is_running) {
    crt_msg_env = (MSG_BUF*) request_memory_block();
    crt_msg_env->mtype = CRT_DISPLAY;

    for(i = 0; i < 8; i++) {
      crt_msg_env->mtext[i] = wall_time[i];
    }
    send_message(PID_CRT, crt_msg_env);
  }
}

//private helper function
char helper_increment(int index) {
  char c = wall_time[index];
  if(c == '0') wall_time[index] = '1';
  else if(c == '1') wall_time[index] = '2';
  else if(c == '2') wall_time[index] = '3';
  else if(c == '3') wall_time[index] = '4';
  else if(c == '4') wall_time[index] = '5';
  else if(c == '5') wall_time[index] = '6';
  else if(c == '6') wall_time[index] = '0';

  return wall_time[index];
}
