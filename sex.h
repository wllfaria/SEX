#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

void daemonize();
void playerctl_status(FILE *status);
bool is_playing_music(char *output);
