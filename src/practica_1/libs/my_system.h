#ifndef _MY_SYSTEM_

// Prototypes for system.c
    int foreground_call(char* cmd, char* args[]);
    int background_call(char* cmd, char* args[]);

// Define MESSAGE number
    #define MESSAGE_OK          0
    #define MESSAGE_KILL_PARENT 101
    #define EXE_DIR "/var/vagrant/src/practica_1/exe"
    #define EXE_SH "./var/vagrant/src/practica_1/exe/sh"

// Define to do not pass thru this more than once
    #define _MY_SYSTEM_ "_MY_SYSTEM_"
#endif
