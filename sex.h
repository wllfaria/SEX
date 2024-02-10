#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

void daemonize();
FILE *playerctl_status();
bool is_playing_music(char *output);
float read_total_time();
void store_session(time_t seconds);
int is_sex_running(const char *cmd);
