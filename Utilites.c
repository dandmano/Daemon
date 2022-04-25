#include "Utilites.h"

//Funkcja sprawdza czy plik ma prawa odczytu i pisania dla grupy
int check_file_perm(const struct stat path_stat)
{
	mode_t perm = path_stat.st_mode;
	//nie ma praw
	if (!(perm & S_IRGRP) || !(perm & S_IXGRP))return 1;
	//ma prawa
	return 0;
}

//Funkcja zwraca 1 je¿eli element jest katalogiem i 0 w przeciwnym wypadku
int is_directory(const struct stat path_stat) {
	return S_ISDIR(path_stat.st_mode);
}

//Funkcja wypisuj¹ca wymagane dane do sysloga
void log_success(char* fulldir, char* pattern)
{
	//Wydobycie i przekonwertowanie czasu i daty na tekst
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char dateandtime[100];
	sprintf(dateandtime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	//Wypisanie informacji o znalezieniu pasuj¹cej nazwy do sysloga
	syslog(LOG_INFO, "SUCCESS!   Time: %s   File Directory: %s   Pattern: %s", dateandtime, fulldir, pattern);
}
