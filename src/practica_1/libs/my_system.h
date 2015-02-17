#ifndef _MY_SYSTEM_

// Prototypes for system.c
    int foreground_call(char* cmd, char* args[]);
    int background_call(char* cmd, char* args[]);

// Define MESSAGE number
    #define MESSAGE_OK              0
    #define MESSAGE_KILL_PARENT     101
    #define MESSAGE_CMD_NOT_FOUND   104

// Define to do not pass thru this more than once
    #define _MY_SYSTEM_ "_MY_SYSTEM_"
#endif
