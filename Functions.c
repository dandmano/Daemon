#include "Functions.h"
#include "Utilites.h"

//Rekurencyjna funkcja poszukuj¹ca plików posiadaj¹cych podany wzór w nazwie
int search_for_filenames(char* dir, char* pattern, int details_mode) 
{
	//Sprawdzenie, czy nie otrzymano sygna³u 
	if (signal1_recieved||signal2_recieved)return 0;

	//Przejœcie deamonem do aktualnie przegl¹danej lokalizacji
	if ((chdir(dir)) < 0) {
		syslog(LOG_INFO, "ERROR! Directory change error after enter %s",dir);
		exit(EXIT_FAILURE);
	}
	//Otwarcie folderu
	DIR* dfd;
	if ((dfd = opendir(dir)) == NULL)
	{
		syslog(LOG_INFO, "ERROR! Nie mo¿na otworzyæ folderu: %s", dir);
		return -1;
	}

	//Przechodzenie po ka¿dym pliku/katalogu w folderze
	struct dirent* dp;
	
	while ((dp = readdir(dfd)) != NULL)
	{
		if (signal1_recieved||signal2_recieved)return 0;

		char* filename[FILENAME_MAX];
		char* fulldir[PATH_MAX];

		//Pozyskanie nazwy aktualnie przegl¹danego pliku
		strcpy(filename, dp->d_name);

		//Pozyskanie pe³nej œcie¿ki
		snprintf(fulldir, sizeof(fulldir), "%s/%s", dir, filename);

		//Pominiêcie zapêtlonych folderów
		if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)continue;

		//Pozyskanie informacji o pliku
		struct stat path_stat;
		if (lstat(filename, &path_stat) == -1)
		{
			syslog(LOG_INFO, "Nie mo¿na uzyskaæ informacji na temat pliku %s%s", dir, filename);
			continue;
		}
		//Pomijanie syslinków
		if (is_syslink(path_stat))continue;

		//Sprawdzanie katalogów
		if (is_directory(path_stat))
		{
			//Omiñ folder je¿eli grupa nie ma uprawnieñ czytania lub wykonania
			if (check_file_perm(path_stat))continue;

			//Porównaj nazwê ze wzorem
			compare_name_with_pattern(fulldir, pattern, filename, details_mode);

			//Rekurencja
			search_for_filenames(fulldir, pattern, details_mode);

			//Powrót deamonem do lokalizacji z przed rekurencji
			if ((chdir(dir)) < 0) {
				syslog(LOG_INFO, "ERROR! Directory change error after recur %s",dir);
				exit(EXIT_FAILURE);
			}
		}
		else
			compare_name_with_pattern(fulldir, pattern, filename, details_mode);
		
	}
	closedir(dfd);
	return 0;
}

//Funkcja porównuje nazwê pliku ze wzorem z argumentu
void compare_name_with_pattern(char* fulldir, char* pattern,char* filename,const int details_mode) 
{
	if (details_mode)
		//syslog(LOG_INFO, "Comparing '%s' with '%s'", filename, pattern); //wylaczone do debug

	//Wypisanie do loga w przypadku znalezienia podci¹gu
	if (strstr(filename, pattern) != NULL)
		log_success(fulldir, pattern);
}