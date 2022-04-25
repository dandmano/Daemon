#pragma once

#include <sys/mman.h>


int search_for_filenames(char* dir, char* pattern, int details_mode);
void compare_name_with_pattern(char* fulldir, char* pattern, char* filename, const int details_mode);