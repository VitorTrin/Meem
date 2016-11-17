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

void printUsage() {
	fprintf(stderr, "This program creates directories\n"
			"Usage: ./mkdir [-mvp] [directory ...]\n"
			"\t-m: set file mode\n"
			"\t-v: verbose\n"
			"\t-p: make parent directories\n"
			"For more information, please consult man\n");
}

char* getFileName(char *path) {
	char *nameStart, *fileName;
	int fileNameLength;

	nameStart = strrchr(path, '/');

	if(nameStart != NULL) {
		return nameStart+1;
	}
}

mode_t getUmask() {
    mode_t mask = umask(0);
    umask (mask);
    return mask;
}

void makeDir(char *path, char *directoryName, bool parents, bool verbose, int mode) {
	DIR *dir;
	struct dirent *entry;
	struct stat statBuffer;
	char auxPath[PATH_MAX], auxDirectoryName[NAME_MAX], *p;

	checkAgain:
	if((dir = opendir(path)) == NULL) {
		if(parents) {
			strcpy(auxPath, path);
			strcpy(auxDirectoryName, getFileName(path));

			p = strrchr(auxPath, '/');
			*p = '\0';

			makeDir(auxPath, auxDirectoryName, parents, verbose, mode);

			goto checkAgain;
		} else {
			fprintf(stderr, "Cannot open directory: %s\n", path);
			return;
		}
	}

	chdir(path);

	while((entry = readdir(dir)) != NULL) {
		lstat(entry->d_name, &statBuffer);

		if(S_ISDIR(statBuffer.st_mode)) {
			if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
				continue;

			if(strcmp(directoryName, entry->d_name) == 0) {
				fprintf(stderr, "ERROR: The directory '%s' already exists.\n", directoryName);
				return;
			}
		}
	}
	if(mode == -1) {
		mode_t mask = getUmask();
		mode = 0777-mask;
	} else
		printf("MODE = %d\n", mode);

	if(mkdir(directoryName, mode) == 0) {
		if(verbose) {
			strcat(path, "/");
			strcat(path, directoryName);
			printf("Directory '%s' created.\n", path);
		}
	} else
		fprintf(stderr, "ERROR %d: %s\n", errno, strerror(errno));

	chdir("..");
	closedir(dir);
}

int main(int argc, char *argv[]) {
	int option, mode = -1;
	char path[PATH_MAX], *directoryName, *auxPath, *endPointer;
	bool parents, verbose;

	getcwd(path, sizeof(path));

	if(argc == 1) {
		printUsage();
		return -1;
	}

	while((option = getopt(argc, argv, "m:vp")) != -1) {
		switch(option) {
			case 'm':
				if(optarg[0] != '=' || (mode = strtol(++optarg, &endPointer, 8)) == 0) {
					fprintf(stderr, "Invalid 'm' option - expecting a number.\n");
					return -1;
				}
				if(strlen(optarg) > 4 || mode > 4095 || *endPointer) {
					fprintf(stderr, "Invalid mode '=%s'\n", optarg);
					return -1;
				}
				break;
			case 'v':
				verbose = true;
				break;
			case 'p':
				parents = true;
				break;
			default:
				printUsage();
				break;
		}
	}
	directoryName = argv[optind];

	if(parents) {
		strcat(path, "/");
		strcat(path, directoryName);
		auxPath = strrchr(path, '/');
		*auxPath = '\0';
		makeDir(path, getFileName(directoryName), parents, verbose, mode);
	} else {
		makeDir(path, directoryName, parents, verbose, mode);
	}

	return 0;
}