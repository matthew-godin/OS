#ifndef K_COMMAND_MAP_H_
#define K_COMMAND_MAP_H_


typedef struct command
{
  char cmd;
  int m_pid;
} COMMAND;

COMMAND registered_cmds[26] = {{0,0}};

void insert_cmd(char cmd, int pid);
int get_pid_from_cmd(char cmd);
void command_map_init(void);

#endif
