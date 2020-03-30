#ifndef WALL_PROC_H_
#define WALL_PROC_H_

#include "common.h"

void update_wall_time(MSG_BUF* cmd_str);
void increment_wall_time(void);
char helper_increment(int index);
void reset_wall_time(void);
void set_wall_str(void);
#endif
