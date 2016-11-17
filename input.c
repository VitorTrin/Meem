#include <stdbool.h>
#include <stdio.h>

void clearInputBuffer() {
	char c;

	while((c = getchar()) != '\n' && c != EOF) {}
}

bool invalidInput(char input) {
	if(input != 'n' && input != 'N' && input != 'y' && input != 'Y')
		return true;
	else
		return false;
}

bool positiveInput(char input) {
	if(input == 'y' || input == 'Y') {
		clearInputBuffer();
		return true;
	} else
		return false;
}

bool negativeInput(char input) {
		if(input == 'n' || input == 'N') {
		clearInputBuffer();
		return true;
	} else
		return false;
}