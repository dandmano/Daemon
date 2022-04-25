#pragma once

#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

extern int details_mode;
extern int is_sleeping;
extern int sleep_time;
extern int signal1_recieved;
extern int signal2_recieved;

int check_file_perm(const struct stat path_stat);
int is_directory(const struct stat path_stat);
int is_syslink(const struct stat path_stat);
void log_success(char* fulldir, char* pattern);
void signal_handler(int signum);