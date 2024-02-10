#include "sex.h"

#define EXIT_FAIL 1
#define EXIT_SUCCESS 0
#define PLAYERCTL_PLAYING "Playing"
#define BUFFER_SIZE 100

int main(void) {
  // daemonize();

  bool was_playing = false;
  time_t start_time;
  FILE *status = playerctl_status();
  char *output = malloc(BUFFER_SIZE);

  if (output == NULL) {
    perror("failed to allocate memory for output");
    exit(EXIT_FAIL);
  }

  while (fgets(output, sizeof(output) - 1, status) != NULL) {
    bool is_playing = is_playing_music(output);
    // we were playing music but now we stopped.
    // so we should calculate and store the total time spent listening
    if (!is_playing && was_playing) {
      syslog(LOG_INFO, "Listening session ended... Storing total time\n");
      was_playing = was_playing;
      time_t end_time = time(NULL);
      time_t delta = end_time - start_time;
      printf("Total listening time: %li seconds\n", delta);
    }
    // we weren't playing music, and now we are
    // so we should start counting the time.
    else if (is_playing && !was_playing) {
      syslog(LOG_INFO, "Listening session started... Starting timer\n");
      was_playing = is_playing;
      time(&start_time);
    }
    // if the status is either of these:
    // is_playing && was_playing
    // !is_playing && !was_playing
    // we don't have to do nothing.
  }

  pclose(status);
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
  FILE *status = popen("playerctl --follow status", "r");
  if (status == NULL) {
    syslog(LOG_ERR, "failed to run playerctl status command: %s\n",
           strerror(errno));
    exit(EXIT_FAIL);
  }
  return status;
}

bool is_playing_music(char *output) {
  return strstr(output, PLAYERCTL_PLAYING) != NULL;
}
