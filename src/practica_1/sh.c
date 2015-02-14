#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_system.h"

#define MAX_LEN_CALL 100

// Function prototypes
void get_cmd_with_args(char*);

// Valid commands
int my_exit();
int shutdown();
int export();
int echo();

// Global vars
char cmd[MAX_LEN_CALL];
char args[MAX_LEN_CALL];

typedef struct{
    char* name;
    void (*callback)(char*);
} CALLBACK_STRUCT;

const CALLBACK_STRUCT CMDS[] = {
    {"exit", my_exit},
    {"shutdown", shutdown},
    {"export", export},
    {"echo", echo},
    {NULL, NULL}
};

int main() {
    char flat_call[MAX_LEN_CALL];
    int i = 0;

    while (1) {
        printf("$ ");
        gets(flat_call);
        get_cmd_with_args(flat_call);

        if (strcmp(cmd, "") != 0) {
            for(i = 0; CMDS[i].name != NULL; i++) {
                if (strcmp(cmd, CMDS[i].name) == 0) {
                    (*(CMDS[i].callback))(args);
                }
                 // else {
                    // TODO: create function to pass args as char to char array
                    // my_system(cmd, NULL);
                // }
            }
        }
    }

    return 0;
}

int my_exit() {
    exit(MESSAGE_OK);
}

int shutdown() {
    exit(MESSAGE_KILL_PARENT);
}

int export() {
    printf("%s\n", args);
    return 0;
}

int echo() {
    printf("%s\n", args);
    return 0;
}


/*
 * Returns:
 *      Array[2] - [CMD, args] where args is char*
 *      NULL - ERROR or no command
 */
void get_cmd_with_args(char* flat_call) {
    int cmd_start = -1, cmd_end = -1;
    int args_start = -1;
    int i = 0;

    // Go to the first character that is not a blank spaces
    while (flat_call[i] == ' ') i++;

    // If first char is end of line
    if (flat_call[i] == '\n' || flat_call[i] == '\0') {
        strcpy(cmd, "");
        strcpy(args, "");
        return;
    }

    cmd_start = i;
    while(flat_call[i] != '\0' && flat_call[i] != '\n') {
        if (flat_call[i] == ' ' && cmd_end == -1) {
            cmd_end = i - 1;
        } else if (cmd_end != -1 && args_start == -1) {
            args_start = i;
        }
        i++;
    }

    if (cmd_end != -1) {
        if (args_start != -1) {
            strcpy(cmd, strndup(flat_call + cmd_start, cmd_end - cmd_start + 1));
            strcpy(args, strndup(flat_call + args_start, i - args_start + 1));
        } else {
            strcpy(cmd, strndup(flat_call + cmd_start, cmd_end - cmd_start + 1));
            strcpy(args, "");
        }
    } else {
        strcpy(cmd, strndup(flat_call + cmd_start, i - cmd_start + 1));
        strcpy(args, "");
    }
}
