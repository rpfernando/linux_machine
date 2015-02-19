#include <unistd.h>
#include <stdio.h>
#include "my_system.h"

int open_getty();

int main () {
	FILE* file;
	char status;
	fork();
	fork();

	do {
		open_getty();

		// Check if status is kill_parent
		file = fopen("status.txt", 'r');
		status = fgetc(file);
		fclose(file);

		// Set flag back for other processes
		file = fopen("status.txt", 'w');
		fputc('0', file);
		fclose(file);

	} while (status != '1');
	return MESSAGE_OK;
}

int open_getty() {
    return xterm_call("./exe/getty");
}