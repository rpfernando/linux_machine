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

    return pid;
}

int xterm_call(char* cmd) {
    int status;
    int pid;

    pid = fork();
    if (pid == 0) {
        execl("/usr/bin/xterm", "/usr/bin/xterm", "-e", cmd, NULL);
        // To catch cmd not found
        exit(MESSAGE_CMD_NOT_FOUND);
    }
    waitpid(pid, &status, 0);

    return status/0x100;
}
