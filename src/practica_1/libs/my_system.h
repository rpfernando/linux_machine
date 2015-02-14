#ifndef _MY_SYSTEM_

// Prototypes for system.c
    int my_system(char* cmd, char args[]);

// Define MESSAGE number
    #define MESSAGE_OK          3000
    #define MESSAGE_KILL_PARENT 3001
    #define EXE_DIR "/var/vagrant/src/practica_1/exe"
    #define EXE_SH "./var/vagrant/src/practica_1/exe/sh"

// Define to do not pass thru this more than once
    #define _MY_SYSTEM_ "_MY_SYSTEM_"
#endif
