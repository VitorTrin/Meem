#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "input.h"

void printUsage() {
	fprintf(stderr, "This program removes files or directories\n"
			"Usage: ./rm [-rdiv] [<file/directory> ...]\n"
			"\t-r: recursive\n"
			"\t-d: empty directories\n"
			"\t-i: interactive\n"
			"\t-v: verbose\n"
			"For more information, please consult man\n");
}

bool isDirectory(char *path) {
	struct stat statBuffer;

	if(lstat(path, &statBuffer) != -1) {
		if(S_ISDIR(statBuffer.st_mode))
			return true;
		else
			return false;
	} else
		printf("ERROR %d: %s\n", errno, strerror(errno));
}

bool isEmpty(char *path) {
	int numberOfFiles = 0;

	DIR *dir;
	struct dirent *entry;

	if((dir = opendir(path)) == NULL) {
		printf("ERROR %d: %s\n", errno, strerror(errno));
		return -1;
	}

	while((entry = readdir(dir)) != NULL) {
		if(++numberOfFiles > 2)
			break;
	}

	closedir(dir);
	if(numberOfFiles <=2) //Empty
		return true;
	else
		return false;
}

char* getFileName(char *path) {
	char *nameStart, *fileName;
	int fileNameLength;

	nameStart = strrchr(path, '/');

	if(nameStart != NULL) {
		return nameStart+1;
	}
}

void walkPath(char *path, char *pathHistory, bool isInteractive, bool verbose) {
	DIR *dir;
	struct dirent *entry;
	struct stat statBuffer;
	char answer;
	char *auxPath;
	char tmp[1024];
	int length;

	if((dir = opendir(path)) == NULL) {
		printf("ERROR %d: %s\n", errno, strerror(errno));
		return;
	}

	auxPath = pathHistory + strlen(pathHistory);

	chdir(path);

	while((entry = readdir(dir)) != NULL) {
		lstat(entry->d_name, &statBuffer);

		if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			continue;

		strcpy(auxPath, entry->d_name);
		
		if(S_ISDIR(statBuffer.st_mode)) {
			if(isInteractive) {
				printf("Do you want to descend to directory '%s'? (y/n) - ", pathHistory);
				answer = getc(stdin);
				//Checking user input
				if(invalidInput(answer))
					return;
				//If negative, skip to the next file/directory
				if(negativeInput(answer))
					continue;
				//If positive, check files inside
				if(positiveInput(answer)) {
					strncpy(tmp, pathHistory, sizeof(tmp));
					strcat(pathHistory, "/");

					walkPath(entry->d_name, pathHistory, isInteractive, verbose);
					
					printf("Do you want to remove the directory '%s'? (y/n) - ", tmp);
					answer = getc(stdin);
					if(invalidInput(answer))
						return;
					if(negativeInput(answer))
						continue;
					if(positiveInput(answer) && verbose)
						printf("You removed the directory '%s'.\n", tmp);
				}
			} else
				if(verbose) {
					strncpy(tmp, pathHistory, sizeof(tmp));
					strcat(pathHistory, "/");
					walkPath(entry->d_name, pathHistory, isInteractive, verbose);
					printf("You removed the directory '%s'.\n", tmp);
				}
		} else
			if(isInteractive) {
				printf("Do you want to remove the file '%s'? (y/n) - ", pathHistory);
				answer = getchar();
				//Checking user input
				if(invalidInput(answer))
					return;
				//If negative, skipsto the next file/directory
				if(negativeInput(answer))
					continue;
				//If positive, remove the file
				if(positiveInput(answer) && verbose)
					printf("You removed the file '%s'.\n", pathHistory);
			} else
				if(verbose)
					printf("You removed the file '%s'.\n", pathHistory);
	}

	chdir("..");
	closedir(dir);
}

void removeFunction(char *path, bool isInteractive, bool verbose, bool ableToRemoveDirectory) {
	if(isDirectory(path)) {
		if(!ableToRemoveDirectory) {
			printf("ERROR: You cannot remove a directory.\n");
			return;
		}
		else 
			if(!isEmpty(path)) {
				printf("ERROR: The directory is not empty.\n");
				return;
		}
	}

	char *fileName = getFileName(path);

	if(isInteractive) {
		char answer;
		printf("Do you want to remove the file '%s'? (y/n) - ", fileName);
		answer = getc(stdin);
		if(invalidInput(answer))
			return;
		else
			if(positiveInput(answer) && verbose) {
				printf("You removed the file '%s'.\n", fileName);
			}
	}

	if(!isInteractive && verbose)
		printf("You removed '%s'.\n", fileName);
}

void removeRecursivelyFunction(char *path, bool isInteractive, bool verbose) {
	char *directoryName, *fullPath, *auxPath;
	int length;

	directoryName = getFileName(path);
	length = strlen(directoryName);

	//Allocating memory for the path
	if((fullPath = malloc (length + NAME_MAX + 2)) == NULL) {
		printf("ERROR %d: %s\n", errno, strerror(errno));
		return;
	}

	memcpy(fullPath, directoryName, length);
	auxPath = fullPath + length; *auxPath++ = '/';

	walkPath(path, fullPath, isInteractive, verbose);

	free(fullPath);
}

int main(int argc, char *argv[]) {
	//removeRecursivelyFunction("/home/iroque/Documents/SO2/teste", true, true);
	int option;
	char path[1024];
	getcwd(path, sizeof(path));
	bool isInteractive, verbose, isRecursive, ableToRemoveDirectory;

	if(argc == 1) {
		printUsage();
		return -1;
	}

	//argc needs to be = 3
	if(argc > 3) {
		printUsage();
		return -1;
	}

	strcat(path, "/");

	while((option = getopt(argc, argv, "rdiv")) != -1) {
		switch(option) {
			case 'r':
				isRecursive = true;
				break;
			case 'd':
				ableToRemoveDirectory = true;
				break;
			case 'i':
				isInteractive = true;
				break;
			case 'v':
				verbose = true;
				break;
			default:
				printUsage();
				break;
		}
	}

	if(optind == 1 && argc == 2) { //No option
		strcat(path, argv[1]);
		removeFunction(path, isInteractive, verbose, ableToRemoveDirectory);
	} else {
		if(argc == 3) {
			strcat(path, argv[2]);

			if(isRecursive)
				removeRecursivelyFunction(path, isInteractive, verbose);
			else
				removeFunction(path, isInteractive, verbose, ableToRemoveDirectory);
		}
	}

	return 0;
}