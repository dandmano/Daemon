#include "Utilites.h"

//Funkcja zwraca nazwê aktualnie przegl¹danego pliku
char* get_filename(struct dirent* dp)
{
	char* filename = malloc(strlen(dp->d_name)*sizeof(char));
	strcpy(filename, dp->d_name);
	return filename;
}

//Funkcja sprawdza czy plik ma prawa odczytu i pisania dla grupy
int check_file_perm(const struct stat path_stat)
{
	mode_t perm = path_stat.st_mode;
	//nie ma praw
	if (!(perm & S_IRGRP) || !(perm & S_IXGRP))return 1;
	//ma prawa
	return 0;
}

int is_directory(const struct stat path_stat) {
	return S_ISDIR(path_stat.st_mode);
}

void log_success(char* fulldir, char* pattern)
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char dateandtime[100];
	sprintf(dateandtime, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	syslog(LOG_INFO, "SUCCESS!\nTime: %s\nFile Directory: %s\nPattern: %s", dateandtime, fulldir, pattern);
}

char* fix_dir(char* dir) 
{
	char* fixed_path = malloc((strlen(dir) + 1) * sizeof(char));
	strcpy(fixed_path, dir);
	strcat(fixed_path, "/");
	return fixed_path;
}

char* get_full_dir(char* dir, char* filename)
{
	char* fulldir = malloc((strlen(dir) + strlen(filename) + 1) * sizeof(char));
	strcpy(fulldir, dir);
	strcat(fulldir, "/");
	strcat(fulldir, filename);
	return fulldir;
}