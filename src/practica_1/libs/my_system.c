#include <unistd.h>
#include "my_system.h"

int my_system(char* cmd, char args[]) {
    int status;
    int pid;

    pid = fork();

    if(pid == 0) {
        printf("Start cmd: %s\n", cmd);
        execv(cmd, args);
    }
    wait(&status);

    return status;
}
