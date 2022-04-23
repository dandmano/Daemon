#include "Headers.h"

//ps -xj | grep Daemon - komenda do sprawdzenia dzialania
//"Usage: %s [-v] [-s sleeptimeinsec] substring1 substring2 ..."

int details_mode = 0; //-v tryb wyświetlający szczegóły
int sleep_time = 120; //czas uśpienia daemona, domyślnie 120s
char log_message[100]; //zmienna do zapisywania sformatowanych string wysyłanych do sysloga

int search_for_filenames(char* dir, char* pattern);

int main(int argc, char* argv[]) {
	
	//Sprawdzenie czy zostały podane wszystkie wymagane argumenty
	if (argc < 2) 
	{
		syslog(LOG_INFO, "ERROR! Not enough arguments");
		exit(EXIT_FAILURE);
	}

	int c;
	//Pętla zajmuje się opcjonalnymi argumentami
	while ((c = getopt(argc, argv, "vs:")) != -1) 
	{
		switch (c)
		{
		case 'v': //argument trybu szczegółowego
			details_mode = 1;
			break;
		case 's': //argument czasu uśpienia
			int tmp = atoi(optarg);
			if (tmp <= 0) {
				syslog(LOG_INFO, "ERROR! Expected sleep time value after -s argument");
				exit(EXIT_FAILURE);
			}
		default:
			syslog(LOG_INFO, "ERROR! getopt default");
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

	//Przejście do lokalizacji początkowej
	if ((chdir("/")) < 0) {
		syslog(LOG_INFO, "ERROR! Directory change error");
		exit(EXIT_FAILURE);
	}

	//Zamknięcie standardowych deskryptorów plików
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	//Pozyskanie ścieżki, w której aktualnie znajduje się daemon
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		syslog(LOG_INFO, "ERROR! Getcwd error");
		exit(EXIT_FAILURE);
	}

	//Pętla główna deamona
	while (1) {
		int status;
		if (details_mode)
			syslog(LOG_INFO, "Daemon awake. Searching for files with names containg given substrings began.");
		for (int i = optind; i <= argc; i++) {
			if (search_for_filenames(cwd,argv[i]) == -1)
			{
				syslog(LOG_INFO, "ERROR! Searching for names exited with -1");
				exit(EXIT_FAILURE);
			}
		}
		if (details_mode)
		{
			sprintf(log_message, "Daemon sleeping for %d", sleep_time);
			syslog(LOG_INFO,log_message);
		}
			
		sleep(sleep_time);
	}
	exit(EXIT_SUCCESS);
}

//Rekurencyjna funkcja poszukująca plików posiadających podany wzór w nazwie
int search_for_filenames(char* dir,char* pattern) {
	struct dirent* dp; //wskaźnik na lokalizację
	DIR* dfd;

	//otwarcie folderu
	if ((dfd = opendir(dir)) == NULL)
	{
		sprintf(log_message, "ERROR! Nie można otworzyć folderu: %s", dir);
		syslog(LOG_INFO,log_message);
		return -1;
	}

	char filename[FILENAME_MAX];
	//Przechodzenie po każdym pliku/katalogu w folderze
	while ((dp = readdir(dfd)) != NULL)
	{
		struct stat stbuf;
		sprintf(filename, "%s/%s", dir, dp->d_name);
		if (stat(filename, &stbuf) == 0)
		{
			mode_t perm = stbuf.st_mode;
			//Omiń plik jeżeli grupa nie ma uprawnień czytania lub wykonania
			if (!(perm & S_IRGRP) || !(perm & S_IXGRP))
				continue;
		}
		else {
			sprintf(log_message, "Nie można uzyskać informacji na temat pliku %s%s", dir, filename);
			continue;
		}
		//Sprawdzanie katalogów
		if ((stbuf.st_mode & S_IFMT) == S_IFDIR)
		{

		}
		else
		{
			
		}
	}
}