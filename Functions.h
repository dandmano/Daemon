#pragma once

#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

int search_for_filenames(char* dir, char* pattern, int details_mode);
void compare_name_with_pattern(char* fulldir, char* pattern, char* filename, const int details_mode);