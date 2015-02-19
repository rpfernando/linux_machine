#include <unistd.h>
#include "my_system.h"

int open_getty();

int main () {
	fork();
	fork();
	while (open_getty() != MESSAGE_KILL_PARENT);
	return MESSAGE_OK;
}

int open_getty() {
    return xterm_call("./exe/getty");
}