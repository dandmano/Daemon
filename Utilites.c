#include "Utilites.h"

int details_mode = 0; //-v tryb wy�wietlaj�cy szczeg�y
int is_sleeping = 0; //0-daemon przeszukuje, 1-daemon �pi
int sleep_time = 120; //czas u�pienia daemona, domy�lnie 120s
int signal1_recieved = 0; //0-nieotrzymano sygna�u 1-otrzymano sygna�
int signal2_recieved = 0; //0-nieotrzymano sygna�u 1-otrzymano sygna�

struct process {
	pid_t pid;
};

//Funkcja sprawdza czy plik ma prawa odczytu i pisania dla grupy
int check_file_perm(const struct stat path_stat)
{
	mode_t perm = path_stat.st_mode;
	//nie ma praw
	if (!(perm & S_IROTH) || !(perm & S_IXOTH))return 1;
	//ma prawa
	return 0;
}

//Funkcja zwraca 1 je�eli element jest katalogiem i 0 w przeciwnym wypadku
int is_directory(const struct stat path_stat) {
	return S_ISDIR(path_stat.st_mode);
}

//Funkcja zwraca 1 je�eli element jest syslinkiem i 0 w przeciwnym wypadku
int is_syslink(const struct stat path_stat) {
	return (S_ISLNK(path_stat.st_mode));
}
//Funkcja wypisuj�ca wymagane dane do sysloga
void log_success(char* fulldir, char* pattern)
{
	//Wydobycie i przekonwertowanie czasu i daty na tekst
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char dateandtime[100];
	sprintf(dateandtime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	//Wypisanie informacji o znalezieniu pasuj�cej nazwy do sysloga
	syslog(LOG_INFO, "SUCCESS!   Time: %s   File Directory: %s   Pattern: %s", dateandtime, fulldir, pattern);
}

//Obs�uga sygna��w
void signal_handler(int signum)
{
	//W wypadku wykrycia sigusr1 daemon w zale�no�ci od wykonwynaje czynno�ci, budzi si� i zaczyna poszukiwanie, lub restartuje poszukiwanie
	if (signum == SIGUSR1)
	{
		if (is_sleeping == 1)
		{
			is_sleeping = 0;
			if(details_mode)syslog(LOG_INFO, "Signal Recieved, Daemon Awoken.");
		}
		else 
		{
			signal1_recieved = 1;
		}
	}
	//W wypadku wykrycia sigusr2 daemon, je�eli nie �pi to przestaje szuka� i si� usypia.
	else if (signum == SIGUSR2)
	{
		if (is_sleeping == 0) 
		{
			signal2_recieved = 1;
		}
	}
}