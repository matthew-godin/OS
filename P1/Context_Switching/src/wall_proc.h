#ifndef WALL_PROC_H_
#define WALL_PROC_H_

char* wall_time = "00:00:00";
int wall_is_running = 0; //wall is initially not running
void update_wall_time(char* cmd_str);
void increment_wall_time(void);
char helper_increment(int index);
#endif
