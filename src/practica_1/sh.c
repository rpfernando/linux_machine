#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_system.h"

#define MAX_LEN_CALL 100
#define MAX_ENV_VARS 50

// Function prototypes
void get_cmd_with_args(char*);
int args_str_to_array(char**);
void free_args_array(char**, int);
int system_function();
void substitute_with_env_vars(char* in);
void substitute(char* in, char* with, int start, int len);

// Commands callbacks
int my_exit();
int shutdown();
int export();
int echo();

// Global vars
char* environment_vars[MAX_ENV_VARS];
char* environment_vars_value[MAX_ENV_VARS];
char* PATH = NULL;
int environment_vars_count = 0;

char cmd_global[MAX_LEN_CALL];
char args_global[MAX_LEN_CALL];
int args_len_global = -1;

typedef struct{
    char* name;
    int (*callback)();
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
    int has_callback = 0;

    while (1) {
        printf("$ ");
        gets(flat_call);
        substitute_with_env_vars(flat_call);
        get_cmd_with_args(flat_call);

        if (strcmp(cmd_global, "") != 0) {
            has_callback = 0;
            for(i = 0; CMDS[i].name != NULL; i++) {
                if (strcmp(cmd_global, CMDS[i].name) == 0) {
                    (*(CMDS[i].callback))();
                    has_callback = 1;
                    break;
                }
            }
            if (!has_callback) {
                if(system_function() == MESSAGE_CMD_NOT_FOUND) {
                    printf("Command not found.\n");
                }
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

/*
 * Returns:
 *       - (0) for no errors.
 *       - (-1) for malformed arguments.
 * When the array is no longer needed "free_args_array" must be called.
 */
int export() {
    char* args_aux;
    char* env_var;
    char* env_var_value;
    int i, len;

    args_aux = args_global;

    for(len = 0; args_aux[len] != ' ' && args_aux[len] != '='; len++) {
        if(args_aux[len] == '\0') return -1;
    }
    env_var = strndup(args_aux, len);

    for(args_aux += len; *args_aux == ' ' || *args_aux == '='; args_aux++) {
        if(*args_aux == '\0') return -1;
    }

    for(len = 0; args_aux[len] != ' ' && args_aux[len] != '\0'; len++);
    env_var_value = strndup(args_aux, len);

    if(strcmp(env_var, "PATH") == 0) {
        PATH = env_var_value;
    }

    for(i = 0; i < environment_vars_count; i++) {
        if(strcmp(environment_vars[i], env_var) == 0) {
            environment_vars_value[i] = env_var_value;
            return MESSAGE_OK;
        }
    }

    environment_vars[environment_vars_count] = env_var;
    environment_vars_value[environment_vars_count] = env_var_value;
    environment_vars_count++;

    return MESSAGE_OK;
}

int echo() {
    printf("%s\n", args_global);
    return MESSAGE_OK;
}

/*
 * Calls function if there was no callback registered.
 */
int system_function(){
    char* vars[MAX_LEN_CALL];
    char cmd_with_path[MAX_LEN_CALL];
    int vars_len, status;

    vars_len = args_str_to_array(vars);

    if (vars_len == -1) {
        printf("Malformed argument.\n");
    } else if (args_len_global != -1 && vars[vars_len-1][0] == '&') {
        vars[vars_len-1] = NULL;
        background_call(cmd_global, vars);
        status = 0;
    } else {
        strcpy(cmd_with_path, "./");
        strcat(cmd_with_path, cmd_global);
        status = foreground_call(cmd_with_path, vars);
        if(status == MESSAGE_CMD_NOT_FOUND && PATH != NULL) {
            strcpy(cmd_with_path, PATH);
            strcat(cmd_with_path, cmd_global);
            status = foreground_call(cmd_with_path, vars);
        }
    }

    free_args_array(vars, vars_len);
    return status;
}

/*
 * Change global variables cmd_global and args_global:
 * Return cmd_global values:
 *      First word in flat call
 *      "" - ERROR or no command
 * Return args_global values:
 *      Second word untill end of line
 *      "" - ERROR or no arguments
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
        args_len_global = -1;
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

/*
 * Returns:
 *       - The count of args in the array.
 *       - (-1) for malformed arguments.
 * When the array is no longer needed "free_args_array" must be called.
 */
int args_str_to_array(char** vars) {
    char* args_aux;
    char quote_mark;
    int vars_len, j;

    vars[0] = cmd_global;
    args_aux = args_global;
    for(vars_len = 1, j = 0; args_aux[0] != '\0'; j = 0, vars_len++) {
        if(args_aux[0] == '"' || args_aux[0] == '\'') {
            quote_mark = args_aux[0];
            for(args_aux += 1; args_aux[j] != quote_mark; j++) {
                if (args_aux[j] == '\0') return -1;
            }
            vars[vars_len] = strndup(args_aux, j);
        } else {
            while(args_aux[0] == ' ') args_aux++;
            while(args_aux[j] != ' ' && args_aux[j] != '\0') j++;
            vars[vars_len] = strndup(args_aux, j);
            if (args_aux[j] == '\0') j--;
        }
        args_aux += j + 1;
    }
    vars[vars_len] = NULL;

    return vars_len;
}

/*
 * Free the memory allocated by spliting the string into an array.
 */
void free_args_array(char ** vars, int vars_len) {
    int j;
    for(j = 1; j < vars_len + 1; j++) {
        free(vars[j]);
        vars[j] = NULL;
    }
}

void substitute_with_env_vars(char* in) {
    int i, j, k;
    char* env_var;
    for(i = 0; in[i] != '\0'; i++) {
        if(in[i] == '$') {
            j = i;
            while(in[j] != ' ' && in[j] != '\0') j++;
            env_var  = strndup(in + i + 1, j - i - 1);
            for(k = 0; k < environment_vars_count; k++) {
                if(strcmp(env_var, environment_vars[k]) == 0) {
                    substitute(in, environment_vars_value[k], i, j - i);
                    break;
                }
            }

            i = j;
            free(env_var);
        }
    }
}

void substitute(char* in, char* with, int start, int len) {
    int with_len;
    int i;

    with_len = strlen(with);
    if(with_len <= len) {
        for(i = start; i < start + with_len; i++) {
            in[i] = with[i - start];
        }
        for(;in[i - 1] != '\0'; i++) in[i] = in[i + len - with_len];
    } else {
        i = start + len;
        while(in[i] != '\0') i++;
        i += with_len - len;
        for(;i > start + len; i--) in[i] = in[i + len - with_len];
        for(i = start; i < start + with_len; i++) in[i] = with[i - start];
    }
}
