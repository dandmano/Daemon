#pragma once

#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
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
extern pid_t* child_processes;
extern int amount_of_processes;
extern pid_t curr_pid;

int check_file_perm(const struct stat path_stat);
int is_directory(const struct stat path_stat);
int is_syslink(const struct stat path_stat);
void log_success(char* fulldir, char* pattern);
void signal_handler_child(int signum);
void signal_handler_supervisor(int signum);