#include <unistd.h>
#include <stdlib.h>
#include "my_system.h"

int foreground_call(char* cmd, char* args[]) {
    int status;
    int pid;

    pid = fork();

    if(pid == 0) {
        execvp(cmd, args);
        // To catch cmd not found
        exit(MESSAGE_CMD_NOT_FOUND);
    }
    waitpid(pid, &status, 0);

    return status/0x100;
}

int background_call(char* cmd, char* args[]) {
    int pid;

    pid = fork();
    if (pid == 0) {
        execvp(cmd, args);
        // To catch cmd not found
        exit(MESSAGE_CMD_NOT_FOUND);
    }

    return MESSAGE_OK;
}
