#include "sex.h"

#define EXIT_FAIL 1
#define EXIT_SUCCESS 0
#define PLAYERCTL_PLAYING "Playing"
#define BUFFER_SIZE 8

int main(void) {
  daemonize();

  signal(SIGTERM, gracefully_stop);

  bool was_playing = false;
  time_t start_time;
  FILE *status = playerctl_status();
  char *output = malloc(BUFFER_SIZE);

  if (output == NULL) {
    perror("failed to allocate memory for output");
    exit(EXIT_FAIL);
  }

  while (fgets(output, sizeof(output), status) != NULL) {
    if (output[strlen(output) - 1] == '\n') {
      output[strlen(output) - 1] = '\0';
    }
    if (strlen(output) == 0) {
      continue;
    }

    bool is_playing = is_playing_music(output);

    if (!is_playing && was_playing) {
      syslog(LOG_INFO, "Listening session ended... Storing total time\n");
      was_playing = is_playing;
      time_t end_time = time(NULL);
      time_t delta = end_time - start_time;
      store_session(delta);
      time(&start_time);
    } else if (is_playing && !was_playing) {
      syslog(LOG_INFO, "Listening session started... Starting timer\n");
      was_playing = is_playing;
      time(&start_time);
    }
  }

  pclose(status);
  closelog();
  free(output);
  exit(EXIT_SUCCESS);
}

void daemonize() {
  if (daemon(1, 0) == -1) {
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
  if (strstr(output, PLAYERCTL_PLAYING) != NULL) {
    return true;
  }
  return false;
}

float read_total_time() {
  FILE *file = fopen("sex_diary", "r");
  float total = 0.0;

  if (file == NULL) {
    return total;
  }

  if (fscanf(file, "%f", &total) != 1) {
    total = 0.0;
  }

  fclose(file);
  return total;
}

void store_session(time_t seconds) {
  float minutes = seconds / 60.0;
  float current_total = read_total_time();
  float total = current_total + minutes;

  FILE *file = fopen("sex_diary", "w");
  if (file == NULL) {
    perror("Failed to open sex_diary for writing");
    return;
  }

  fprintf(file, "%.2f\n", total);
  fclose(file);
}

void gracefully_stop(int signum) {
  syslog(LOG_INFO, "SIGTERM received, shutting down...\n");
  closelog();
  exit(EXIT_SUCCESS);
}
