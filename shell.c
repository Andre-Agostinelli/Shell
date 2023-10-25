#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "shell.h"
#include "tokens.h"

// This drives the interactive shell
int main(int argc, char **argv) {
    char input[MAX_INPUT_LENGTH];
    Token* tokens;

    printf("Welcome to mini-shell.\n");

    while (1) {
        printf("shell $ ");

        // Check if we reached the end of file (Ctrl-D)
        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL) {
            if (feof(stdin)) {
                printf("\nBye bye.\n");
                break;
            }
        }

        if (strcmp(input, "exit\n") == 0) {
            printf("Bye bye.\n");
            break;
        }

        tokens = tokenize(input);

        for (int i = 0; *tokens[i].value != '\0'; i++) {
            printf("Token %d: %s, Type: %c\n", i, tokens[i].value, tokens[i].type);
        }

        // Now you can process and execute the command using the tokens.

        free(tokens);
    }

    return 0;
}