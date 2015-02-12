#include <stdio.h>
#include <string.h>

#define MAX_USER_LEN 100
#define MAX_PASS_LEN 100
#define DEFAULT_USER_PASS "root:toor\n"

#define SEPARATOR ":"

int validate(char* user, char* pass);
void open_sh();

int main() {
    char user[MAX_USER_LEN];
    char pass[MAX_PASS_LEN];

    do {
        printf("login: ");
        scanf("%s",&user);

        printf("pass: ");
        scanf("%s",&pass);
    } while (validate(user, pass) != 0);

    open_sh();

    return 0;
}


/* Validates if the user and password are correct using pairs in shadow file.
 *    Assumption, the users are unique in the shadow file.
 * Returns:
 *      0: Valid user, password
 *      1: Incorrect user or password
 */
int validate(char* user, char* pass) {
    FILE *shadow;
    char file_user_pass[MAX_USER_LEN + MAX_PASS_LEN];
    char user_pass[MAX_USER_LEN + MAX_PASS_LEN];

    strcpy(user_pass, user);
    strcat(user_pass, SEPARATOR);
    strcat(user_pass, pass);
    strcat(user_pass, "\n");

    shadow = fopen("shadow","r");
    if (shadow == NULL) {
        printf("File 'shadow' not found\n");
    } else {
        while (!feof(shadow)) {
            fgets(file_user_pass, MAX_USER_LEN + MAX_PASS_LEN, shadow);

            if (strcmp(user_pass, file_user_pass) == 0) {
                return 0;
            }
        }

        fclose(shadow);
    }

    return 1;
}

void open_sh() {

}
