#include "Functions.h"
#include "Utilites.h"

//Rekurencyjna funkcja poszukuj�ca plik�w posiadaj�cych podany wz�r w nazwie
int search_for_filenames(char* dir, char* pattern) 
{
	//Przej�cie deamonem do aktualnie przegl�danej lokalizacji
	if ((chdir(dir)) < 0) {
		syslog(LOG_INFO, "ERROR! Process %d Directory change error after enter %s",curr_pid,dir);
		exit(EXIT_FAILURE);
	}
	//Otwarcie folderu
	DIR* dfd;
	if ((dfd = opendir(dir)) == NULL)
	{
		syslog(LOG_INFO, "ERROR! Process %d,  Nie mo�na otworzy� folderu: %s",curr_pid, dir);
		return -1;
	}

	//Przechodzenie po ka�dym pliku/katalogu w folderze
	struct dirent* dp;
	
	while ((dp = readdir(dfd)) != NULL)
	{
		char filename[256];
		char fulldir[PATH_MAX];

		//Pozyskanie nazwy aktualnie przegl�danego pliku
		strcpy(filename, dp->d_name);

		//Pozyskanie pe�nej �cie�ki
		snprintf(fulldir, sizeof(fulldir), "%s/%s", dir, filename);

		//Pomini�cie zap�tlonych folder�w
		if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)continue;

		//Pozyskanie informacji o pliku
		struct stat path_stat;
		if (lstat(filename, &path_stat) == -1)
		{
			syslog(LOG_INFO, "Process %d Nie mo�na uzyska� informacji na temat pliku %s%s",curr_pid, dir, filename);
			continue;
		}
		//Pomijanie syslink�w
		if (is_syslink(path_stat))continue;

		//Omi� folder je�eli grupa nie ma uprawnie� czytania lub wykonania
		if (check_file_perm(path_stat))continue;

		//Sprawdzanie katalog�w
		if (is_directory(path_stat))
		{
			//Por�wnaj nazw� ze wzorem
			compare_name_with_pattern(fulldir, pattern, filename, details_mode);

			//Rekurencja
			search_for_filenames(fulldir, pattern);

			//Powr�t deamonem do lokalizacji z przed rekurencji
			if ((chdir(dir)) < 0) {
				syslog(LOG_INFO, "ERROR! Process %d Directory change error after recur %s",curr_pid,dir);
				exit(EXIT_FAILURE);
			}
		}
		else
			compare_name_with_pattern(fulldir, pattern, filename, details_mode);
		
	}
	closedir(dfd);
	return 0;
}

//Funkcja por�wnuje nazw� pliku ze wzorem z argumentu
void compare_name_with_pattern(char* fulldir, char* pattern,char* filename,const int details_mode) 
{
	if (details_mode)
		//syslog(LOG_INFO, "Process %d Comparing '%s' with '%s'",curr_pid ,filename, pattern); //wylaczone do debug

	//Wypisanie do loga w przypadku znalezienia podci�gu
	if (strstr(filename, pattern) != NULL)
		log_success(fulldir, pattern);
}