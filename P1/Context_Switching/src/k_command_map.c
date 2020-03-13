#include "k_command_map.h"
#include "k_rtx.h"

COMMAND registered_cmds[26] = {{0,0}};

void insert_cmd(char cmd, int pid) {
  int i;
  for(i = 0; i < 26; i++) {
    // check nothing has been registered with the char cmd
    if(registered_cmds[i].cmd == 0 && registered_cmds[i].m_pid ==0) {
      registered_cmds[i].cmd = cmd;
      registered_cmds[i].m_pid = pid;
      break;
    }
  }
}

int get_pid_from_cmd(char cmd) {
  int i;
  for(i = 0; i < 26; i++) {
    if(registered_cmds[i].cmd == cmd) {
      return registered_cmds[i].m_pid;
    }
  }
  return RTX_ERR; //no pid associated with command
}

void command_map_init(void) {
  int i;
  //initialize all to 0
  for(i = 0; i < 26; i++) {
    registered_cmds[i].cmd = 0;
    registered_cmds[i].m_pid = 0;
  }

  //register wall command
  registered_cmds[0].cmd = 'W';
  registered_cmds[0].m_pid = 11;
}
