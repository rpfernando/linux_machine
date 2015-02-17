#include <unistd.h>
#include "my_system.h"

int foreground_call(char* cmd, char* args[]) {
    int status;
    int pid;

    pid = fork();

    if(pid == 0) {
        execvp(cmd, args);
    }
    waitpid(pid, &status, 0);

    return status/0x100;
}

int background_call(char* cmd, char* args[]) {
    int pid;

    pid = fork();
    if (pid == 0) {
        execvp(cmd, args);
    }

    return MESSAGE_OK;
}
