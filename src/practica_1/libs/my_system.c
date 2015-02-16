#include <unistd.h>
#include "my_system.h"

int my_system(char* cmd, char* args[]) {
    int status;
    int pid;

    pid = fork();

    if(pid == 0) {
        execvp(cmd, args);
    }
    waitpid(pid, &status, 0);

    return status/0x100;
}
