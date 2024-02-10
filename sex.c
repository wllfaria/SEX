#include "sex.h"

#define EXIT_FAIL 1
#define EXIT_SUCCESS 0
#define PLAYERCTL_PLAYING "Playing"
#define FILE_PATH "~/.config/i3/sex_diary"
#define BUFFER_SIZE 8

int main(void) {

  if (is_sex_running("sex") >= 2) {
    exit(0);
  }

  daemonize();

  bool was_playing = false;
  time_t start_time;
  FILE *status = playerctl_status();
  char *output = malloc(BUFFER_SIZE);

  if (output == NULL) {
    perror("failed to allocate memory for output");
    exit(EXIT_FAIL);
  }

  while (fgets(output, BUFFER_SIZE, status) != NULL) {
    if (output[strlen(output) - 1] == '\n') {
      output[strlen(output) - 1] = '\0';
    }
    if (strlen(output) == 0) {
      continue;
    }

    bool is_playing = is_playing_music(output);

    if (!is_playing && was_playing) {
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
  FILE *file = fopen(FILE_PATH, "r");
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
  syslog(LOG_INFO, "Listening session ended... Storing total time: %f\n",
         total);

  FILE *file = fopen(FILE_PATH, "w");
  if (file == NULL) {
    syslog(LOG_ERR, "failed to write to sex_diary: %s", strerror(errno));
    return;
  }

  fprintf(file, "%.2f\n", total);
  fclose(file);
}

int is_sex_running(const char *cmd) {
  char command[256];
  FILE *fp;
  int count = 0;

  snprintf(command, sizeof(command), "pgrep -f %s", cmd);

  fp = popen(command, "r");
  if (fp == NULL) {
    syslog(LOG_ERR, "failed to grep for existing process: %s", strerror(errno));
    exit(EXIT_FAIL);
  }

  char buffer[1024];
  if (fgets(buffer, sizeof(buffer), fp) != NULL) {
    count++;
  }

  pclose(fp);

  return count;
}
