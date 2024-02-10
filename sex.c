
#include "sex.h"

#define EXIT_FAIL 1
#define PLAYERCTL_PLAYING "Playing"

int main(void) {
  daemonize();

  FILE *status;
  char *output;
  bool was_playing = false;
  time_t start_time = 0;

  while (1) {
    playerctl_status(status);

    while (fgets(output, sizeof(output) - 1, status) != NULL) {
      bool is_playing = is_playing_music(output);
      // we were playing music but now we stopped.
      // so we should calculate and store the total time spent listening
      if (!is_playing && was_playing) {
      }
      // we weren't playing music, and now we are
      // so we should start counting the time.
      else if (is_playing && !was_playing) {
      }
      // if the status is either of these:
      // is_playing && was_playing
      // !is_playing && !was_playing
      // we don't have to do nothing.
    }
  }

  closelog();
  exit(0);
}

void daemonize() {
  if (daemon(0, 0) == -1) {
    perror("daemon failed to initialize");
    exit(EXIT_FAIL);
  }

  openlog("SEX", LOG_PID, LOG_DAEMON);
}

void playerctl_status(FILE *status) {
  status = popen("playerctl status", "r");
  if (status == NULL) {
    syslog(LOG_ERR, "failed to run playerctl status command\n");
    exit(EXIT_FAIL);
  }
}

bool is_playing_music(char *output) {
  if (strstr(output, PLAYERCTL_PLAYING)) {
    return true;
  }
  return false;
}
