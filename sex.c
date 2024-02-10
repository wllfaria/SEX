#include "sex.h"

#define EXIT_FAIL 1
#define EXIT_SUCCESS 0
#define PLAYERCTL_PLAYING "Playing"

int main(void) {
  daemonize();

  FILE *status;
  char *output = malloc(100);
  bool was_playing = false;
  time_t start_time = 0;

  while (1) {
    status = playerctl_status();

    while (fgets(output, sizeof(output) - 1, status) != NULL) {
      printf("lol\n");
      bool is_playing = is_playing_music(output);
      // we were playing music but now we stopped.
      // so we should calculate and store the total time spent listening
      if (!is_playing && was_playing) {
        syslog(LOG_INFO, "Listening session ended... Storing total time\n");
      }
      // we weren't playing music, and now we are
      // so we should start counting the time.
      else if (is_playing && !was_playing) {
        syslog(LOG_INFO, "Listening session started... Starting timer\n");
      }
      // if the status is either of these:
      // is_playing && was_playing
      // !is_playing && !was_playing
      // we don't have to do nothing.
    }
  }

  closelog();
  free(output);
  exit(EXIT_SUCCESS);
}

void daemonize() {
  if (daemon(0, 0) == -1) {
    perror("daemon failed to initialize");
    exit(EXIT_FAIL);
  }

  openlog("SEX", LOG_PID, LOG_DAEMON);
}

FILE *playerctl_status() {
  FILE *status = popen("playerctl status", "r");
  if (status == NULL) {
    syslog(LOG_ERR, "failed to run playerctl status command\n");
    exit(EXIT_FAIL);
  }
  return status;
}

bool is_playing_music(char *output) {
  return strstr(output, PLAYERCTL_PLAYING) != NULL;
}
