#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define BLOCK_SIZE 1024

void printUsage() {
	fprintf(stderr, "Manual repository.\n"
			"Usage: ./man [function name]\n"
			"\tAvailable funtions: mkdir, rm, touch\n");
}

void man(char *path) {
	char buffer[BLOCK_SIZE];
	FILE *file;
	size_t nRead;

	if((file = fopen(path, "r")) == NULL) {
		fprintf(stderr, "ERROR %d: %s", errno, strerror(errno));
		return;
	}


	while((nRead = fread(buffer, 1, BLOCK_SIZE, file)) > 0) {
		fwrite(buffer, 1, nRead, stdout);
		if(ferror(file))
			fprintf(stderr, "ERROR %d: %s", errno, strerror(errno));
	}

	fclose(file);
}

int main(int argc, char *argv[]) {
	char functionName[NAME_MAX];
	char functionPath[NAME_MAX*2];

	strcpy(functionPath, "functions manual/");

	if(argc == 1) {
		printUsage();
		return -1;
	}

	if(argc == 2) {
		strcpy(functionName, argv[1]);

		if(strcmp(functionName, "mkdir") == 0 ||
		   strcmp(functionName, "rm") == 0 ||
		   strcmp(functionName, "touch") == 0) {
			strcat(functionName, ".txt");
			strcat(functionPath, functionName);
		}
		else {
			printUsage();
			return -1;
		}
	}

	man(functionPath);
	
	return 0;
}