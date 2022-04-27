#include "Daemon.h"
#include "Functions.h"
#include "Utilites.h"

//ps -xj | grep Daemon - komenda do sprawdzenia działających procesów
// tail -f /var/log/syslog | grep --line-buffered Daemon  - komenda do oglądania logów
//"Usage: %s [-v] [-s sleeptimeinsec] substring1 substring2 ..."

int main(int argc, char* argv[]) 
{

	//Sprawdzenie czy zostały podane wszystkie wymagane argumenty
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: %s [-v] [-s sleeptimeinsec] substring1 substring2 ...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int c;
	int tmp;

	//Pętla zajmuje się opcjonalnymi argumentami
	while ((c = getopt(argc, argv, "vs:")) != -1) 
	{
		switch (c)
		{
		case 'v': //argument trybu szczegółowego
			details_mode = 1;
			break;
		case 's': //argument czasu uśpienia
			tmp = atoi(optarg);
			if (tmp <= 0) {
				fprintf(stderr, "Expected value after -s. Example -s 30\n");
				exit(EXIT_FAILURE);
			}
			sleep_time = tmp;
			break;
		default:
			fprintf(stderr, "Usage: %s [-v] [-s sleeptimeinsec] substring1 substring2 ...\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	//Utworzenie nowego procesu potomnego
	pid_t pid = fork();

	//Sprawdzenie czy nie wystąpił bład
	if (pid < 0)
	{
		syslog(LOG_INFO, "ERROR! Forking error");
		exit(EXIT_FAILURE);
	}

	//Zakończenie działania rodzica
	else if (pid > 0)
		exit(EXIT_SUCCESS);

	//Ustawienie praw dostępu do plików deamona
	umask(0);

	//Ustawienie procesu jako lidera sesji, nadanie sid
	if (setsid() < 0) 
	{
		syslog(LOG_INFO, "ERROR! Setsid error");
		exit(EXIT_FAILURE);
	} 

	//Obsługa sygnałów
	signal(SIGHUP, SIG_IGN);
	signal(SIGUSR1, signal_handler_supervisor);
	signal(SIGUSR2, signal_handler_supervisor);

	//Zamknięcie standardowych deskryptorów plików
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	//Przejście do lokalizacji początkowej
	if ((chdir("/")) < 0) 
	{
		syslog(LOG_INFO, "ERROR! Directory change error");
		exit(EXIT_FAILURE);
	}
	//Ustawienie ścieżki, w której aktualnie znajduje się daemon
	char* cwd = "/";

	//Czyszczenie otwartych deskryptorów plików
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
	{
		close(x);
	}

	

	//Pętla główna deamona
	while (1) {
		//Obudzenie deamona
		is_sleeping = 0;
		if (details_mode)
			syslog(LOG_INFO, "Process -%d- Searching for files with names containg given substrings.",getpid());

		amount_of_processes = argc - optind;
		child_processes = malloc((amount_of_processes) * sizeof(pid_t));

		//Dla każdego argumentu wywoływana jest funkcja poszukująca
		for (int i = optind, x=0; i < argc; i++, x++) 
		{
			pid = fork();
			//Sprawdzenie czy nie wystąpił bład
			if (pid < 0)
			{
				syslog(LOG_INFO, "ERROR! Forking in for error");
				exit(EXIT_FAILURE);
			}
			
			//Zapisanie pid'ów potomnych procesów przez proces nadzorczy
			if(pid>0)child_processes[x] = pid;
			curr_pid = getpid();

			//Zadania dla każdego procesu-dziecka
			//Przekierowanie sygnałow, oraz wywowałnie funkcji wyszukującej
			if (pid == 0)
			{
				signal(SIGUSR1, signal_handler_child);
				signal(SIGUSR2, signal_handler_child);
				free(child_processes);
				if (search_for_filenames(cwd, argv[i], details_mode) == -1)
				{
					syslog(LOG_INFO, "ERROR! Searching for names exited with -1");
					exit(EXIT_FAILURE);
				}
				syslog(LOG_INFO, "KILLING PROCESS! %d ",curr_pid);
				exit(EXIT_SUCCESS);
				syslog(LOG_INFO, "PROCESS NOT KILED!!!EWSADWAD %d", curr_pid);
			}
		}

		int status;
		pid_t tmppid;
		while (tmppid = wait(&status)>0&&!signal1_recieved&&!signal2_recieved);

		//W przypadku otrzymania sigusr1, przeszukiwanie się restartuje
		if (signal1_recieved) 
		{
			signal1_recieved = 0;
			if (details_mode)
				syslog(LOG_INFO, "SIGUSR1 recieved, search restart.");
			continue;
		}

		//W przypadku otrzymania sigusr2, przeszukiwanie się kończy i daemon się usypia
		if (signal2_recieved)
		{
			signal2_recieved = 0;
			if (details_mode)
				syslog(LOG_INFO, "SIGUSR2 recieved, search stopped.");
		}

		if (details_mode)
			syslog(LOG_INFO, "Process -%d- Daemon sleeping for %d",getpid(), sleep_time);

		if(pid>0)
			free(child_processes);
		//Uśpienie deamona
		is_sleeping = 1;
		sleep(sleep_time);
	}
	exit(EXIT_SUCCESS);
}