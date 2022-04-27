#include "Utilites.h"

int details_mode = 0; //-v tryb wyœwietlaj¹cy szczegó³y
int is_sleeping = 0; //0-daemon przeszukuje, 1-daemon œpi
int sleep_time = 120; //czas uœpienia daemona, domyœlnie 120s
int sleep_time_left = 0; //w przypadku obudzenia deamona niechcianym sygna³em, wykorzystywane do dalszego uœpienia
int signal1_recieved = 0; //0-nieotrzymano sygna³u 1-otrzymano sygna³
int signal2_recieved = 0; //0-nieotrzymano sygna³u 1-otrzymano sygna³
pid_t* child_processes; //id procesów potomnych
int amount_of_processes; //iloœæ procesów potomnych
pid_t curr_pid; //id aktualnego procesu

//Funkcja sprawdza czy plik ma prawa odczytu i pisania dla grupy
int check_file_perm(const struct stat path_stat)
{
	mode_t perm = path_stat.st_mode;
	//nie ma praw
	if (!(perm & S_IROTH) || !(perm & S_IXOTH))return 1;
	//ma prawa
	return 0;
}

//Funkcja zwraca 1 je¿eli element jest katalogiem i 0 w przeciwnym wypadku
int is_directory(const struct stat path_stat) {
	return S_ISDIR(path_stat.st_mode);
}

//Funkcja zwraca 1 je¿eli element jest syslinkiem i 0 w przeciwnym wypadku
int is_syslink(const struct stat path_stat) {
	return (S_ISLNK(path_stat.st_mode));
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

//Obs³uga sygna³ów przez proces dziecko
void signal_handler_child(int signum)
{
	if (signum == SIGUSR1|| signum == SIGUSR2)raise(SIGTERM);
}

//Obs³uga sygna³ów przez proces nadzorczy
void signal_handler_supervisor(int signum)
{
	//Je¿eli daemon œpi i otrzyma sigusr1 to siê budzi
	if (is_sleeping)
	{
		if (signum == SIGUSR1)
		{
			is_sleeping = 0;
			if (details_mode)
				syslog(LOG_INFO, "SIGUSR1 recieved, Daemon awoken.");
		}
		//W wypadku gdy daemon zosta³ obudzony przez sygna³ inny ni¿ sigusr1 to kontynuuje spanie
		else if(sleep_time_left!=0)
		{
			sleep_time_left = sleep(sleep_time_left);
		}
	}
	//Je¿eli nie œpi i otrzyma sigusr1 to przesy³a im ten sygna³
	else if (signum == SIGUSR1)
	{ 
		signal1_recieved = 1;
		for (int i = 0; i < amount_of_processes; i++)
			kill(child_processes[i], SIGUSR1);
	}
	//W wypadku wykrycia sigusr2 daemon, je¿eli nie œpi to przesy³a sygna³ potomnym i siê usypia.
	else if (signum == SIGUSR2)
	{
		signal2_recieved = 1;
		for (int i = 0; i < amount_of_processes; i++)
			kill(child_processes[i], SIGUSR2);
	}
}