#pragma once

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

char* get_filename(struct dirent* dp);
int check_file_perm(const struct stat path_stat);
int is_directory(const struct stat path_stat);
void log_success(char* fulldir, char* pattern);
char* fix_dir(char* dir);
char* get_full_dir(char* dir, char* filename);