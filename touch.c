#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#define DEBUG 1
#define VERBOSE 1

void printUsage() {
	fprintf(stderr, "Update the access and modification times of each FILE to the current time.\n"
			"Usage: touch [-mac] [<file/directory> ...]\n"
			"\t -m: change only modification time\n"
			"\t -a: change only access time\n"
			"\t -c: do not create files\n"
			"For more information, please consult man\n");
}

bool isValidPath(char *path) {
	#if DEBUG
		printf("chegou no isValidPath\n");
	#endif

	struct stat statBuffer;

	if(lstat(path, &statBuffer) != -1)
		return true;
	else {
		printf("ERROR %d: %s\n", errno, strerror(errno));
		return false;
	}
}


bool fileExists(char* fullpath) {
	#if DEBUG
		printf("chegou no fileExists\n");
	#endif

	struct stat statBuffer;

	if(lstat(fullpath, &statBuffer) != -1)
		return true;
	else
		return false;

}

void touch (char* path, const char* file, bool access, bool create, bool modification) {
	// struct stat statBuffer;

	char fullpath[1024] = "";
	strcat(fullpath, path);
	strcat(fullpath, "/");
	strcat(fullpath, file);
	time_t agora;
	char hora[1024];
	struct tm* time_info;
	// int fd;

	#if DEBUG
	printf("%s\n",fullpath);
	#endif

	if (isValidPath(path))
	{
		#if DEBUG
		printf("passou do isValidPath\n");
		#endif

		if(fileExists(fullpath)) {
			//tenta abrir
			#if VERBOSE
			printf("Arquivo atualizado\n");
			#endif

			if(access && modification){
				time(&agora);
				time_info = localtime(&agora);
				strftime(hora,1024,"%c",time_info);
				printf("Mudamos o tempo para %s\n", hora);
			}
			if(access && !modification) {
				time(&agora);
				time_info = localtime(&agora);
				strftime(hora,1024,"%c",time_info);
				printf("Mudamos o tempo de acesso para %s\n", hora);
			}
			if(!access && modification) {
				time(&agora);
				time_info = localtime(&agora);
				strftime(hora,1024,"%c",time_info);
				printf("Mudamos o tempo de modificacao para %s\n", hora);
			}

			// muda os acessos
		}
		else {
			if(create){
				#if VERBOSE
				printf("Nao existe e nao vou criar\n");
				#endif
				return;
			}
			else {
			//cria vazia
				#if VERBOSE
				printf("Arquivo criado!\n");
				#endif
			}
		}

	}

	return;

}





int main(int argc, char *argv[])
{
	char path[1024];
	getcwd(path, sizeof(path));
	bool create = false, access = false, modification = false;
	int option;

	//checa argumentos
	if(argc == 1 || argc > 3) {
		printUsage();
		return -1;
	}


	while((option = getopt(argc, argv, "acm")) != -1) {
		switch(option) {
			case 'a':
				access = true;
				break;

			case 'c':
				create = true;
				break;		

			case 'm':
				modification = true;
				break;

			default:
				printUsage();
				break;
		}
	}
	//bota o alvo no final do path
	if(argc == 2) {

		touch(path, argv[1], access,create, modification);
	}

	else
		touch(path, argv[2], access,create, modification);

	return 0;

}