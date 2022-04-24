﻿#include "Daemon.h"
#include "Functions.h"

//ps -xj | grep Daemon - komenda do sprawdzenia działających procesów
// tail -f /var/log/syslog | grep --line-buffered Daemon  - komenda do oglądania logów
//"Usage: %s [-v] [-s sleeptimeinsec] substring1 substring2 ..."

int main(int argc, char* argv[]) {
	
	//Sprawdzenie czy zostały podane wszystkie wymagane argumenty
	if (argc < 2) 
	{
		syslog(LOG_INFO, "ERROR! Not enough arguments");
		exit(EXIT_FAILURE);
	}

	int details_mode = 0; //-v tryb wyświetlający szczegóły
	int sleep_time = 120; //czas uśpienia daemona, domyślnie 120s

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
				syslog(LOG_INFO, "ERROR! Expected sleep time value after -s argument");
				exit(EXIT_FAILURE);
			}
			sleep_time = tmp;
			break;
		default:
			syslog(LOG_INFO, "optget default c=%d", c);
			exit(EXIT_FAILURE);
		}
	}
	/*
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
	} */

	//Przejście do lokalizacji początkowej

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGABRT, SIG_IGN);

	if ((chdir("/home")) < 0) {
		syslog(LOG_INFO, "ERROR! Directory change error");
		exit(EXIT_FAILURE);
	}

	//Zamknięcie standardowych deskryptorów plików
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	//Ustawienie ścieżki, w której aktualnie znajduje się daemon
	char* cwd = "/home";

	//Czyszczenie otwartych deskryptorów plików
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
	{
		close(x);
	}

	//Pętla główna deamona
	while (1) {
		if (details_mode)
			syslog(LOG_INFO, "Daemon awake. Searching for files with names containg given substrings.");

		//Dla każdego argumentu wywoływana jest funkcja poszukująca
		for (int i = optind; i < argc; i++) {
			syslog(LOG_INFO, "Petla argument numer %d czyli %s ", i, argv[i]);//debug
			if (search_for_filenames(cwd,argv[i],details_mode) == -1)
			{
				syslog(LOG_INFO, "ERROR! Errno number %d",errno);//debug
				syslog(LOG_INFO, "ERROR! Searching for names exited with -1");
				exit(EXIT_FAILURE);
			}
		}
		break; //tymczasowo aby nie usuwac ręcznie procesów %%%%
		if (details_mode)
			//syslog(LOG_INFO, "Daemon sleeping for %d", sleep_time);

		//Uśpienie deamona
		sleep(sleep_time);
	}
	exit(EXIT_SUCCESS);
}