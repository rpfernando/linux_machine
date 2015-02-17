#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_system.h"

#define MAX_LEN_CALL 100

// Function prototypes
void get_cmd_with_args(char*);
void system_function();

// Valid commands
int my_exit();
int shutdown();
int export();
int echo();

// Global vars
char cmd_global[MAX_LEN_CALL];
char args_global[MAX_LEN_CALL];
int args_len_global = -1;

typedef struct{
    char* name;
    void (*callback)();
} CALLBACK_STRUCT;

const CALLBACK_STRUCT CMDS[] = {
    {"exit", my_exit},
    {"shutdown", shutdown},
    {NULL, NULL}
};

int main() {
    char flat_call[MAX_LEN_CALL];
    int i = 0;
    int has_callback = 0;

    while (1) {
        printf("$ ");
        gets(flat_call);
        get_cmd_with_args(flat_call);

        if (strcmp(cmd_global, "") != 0) {
            has_callback = 0;
            for(i = 0; CMDS[i].name != NULL; i++) {
                if (strcmp(cmd_global, CMDS[i].name) == 0) {
                    (*(CMDS[i].callback))();
                    has_callback = 1;
                }
            }
            if (!has_callback) system_function();
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
    printf("%s\n", args_global);
    return 0;
}

int echo() {
    printf("%s\n", args_global);
    return 0;
}

void system_function(){
    char* vars[MAX_LEN_CALL];
    char* args_aux;
    int vars_len;
    int j;

    vars[0] = cmd_global;
    args_aux = args_global;
    for(vars_len = 1; args_aux[0] != '\0'; j = 0, vars_len++) {
        while(args_aux[j] != ' ' && args_aux[j] != '\0') j++;
        vars[vars_len] = strndup(args_aux, j);
        args_aux += j + 1;
    }
    vars[vars_len] = NULL;

    if (args_len_global != -1 && vars[vars_len-1][0] == '&') {
        vars[vars_len-1] = NULL;
        background_call(cmd_global, vars);
    } else {
        foreground_call(cmd_global, vars);
    }

    for(j = 1; j < vars_len; j++) free(vars[j]);
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
        strcpy(cmd_global, "");
        strcpy(args_global, "");
        args_len_global -1;
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
            strcpy(cmd_global, strndup(flat_call + cmd_start, cmd_end - cmd_start + 1));
            strcpy(args_global, strndup(flat_call + args_start, i - args_start + 1));
            args_len_global = i - args_start + 1;
            while(args_global[args_len_global] == '\n' ||
                    args_global[args_len_global] == '\0' ||
                    args_global[args_len_global] == ' ') args_len_global--;
            args_global[++args_len_global] = '\0';
        } else {
            strcpy(cmd_global, strndup(flat_call + cmd_start, cmd_end - cmd_start + 1));
            args_len_global = -1;
            strcpy(args_global, "");
        }
    } else {
        strcpy(cmd_global, strndup(flat_call + cmd_start, i - cmd_start + 1));
        args_len_global = -1;
        strcpy(args_global, "");
    }
}
