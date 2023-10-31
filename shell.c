#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "shell.h"
#include "tokens.h"

void execute_tokens(Token* tokens, int start, int end) {
    int i;
    // Handle parentheses
    for (i = start; i <= end; i++) {
        // printf("Checking for parantheses\n");
        if (strcmp(tokens[i].value, "(") == 0) {
            // Find matching closing parenthesis
            int j = i + 1; // Start from the token after the open parenthesis
            int open_count = 1;
            int close_count = 0;
            while (open_count > close_count) {
                if (strcmp(tokens[j].value, "(") == 0) {
                    open_count++;
                } else if (strcmp(tokens[j].value, ")") == 0) {
                    close_count++;
                }
                j++;
            }
            printf("Executing inside parentheses:\n");
            execute_tokens(tokens, i + 1, j - 2); // Execute inside the parentheses
            i = j; // Skip the processed tokens
        }
    }

    // Handle semicolons
    for (i = start; i <= end; i++) {
        // printf("Checking for semicolons\n");
        if (strcmp(tokens[i].value, ";") == 0) {
            // Semicolon found, execute left side, then right side
            printf("Executing left side of semicolon:\n");
            execute_tokens(tokens, start, i - 1); // Left side
            printf("Executing right side of semicolon:\n");
            execute_tokens(tokens, i + 1, end); // Right side
            return;
        }
    }

    // Handle pipes
    for (i = start; i <= end; i++) {
        if (strcmp(tokens[i].value, "|") == 0) {
            // Pipe found, execute left side, then right side
            printf("Executing left side of pipe:\n");
            execute_tokens(tokens, start, i - 1); // Left side
            printf("Executing right side of pipe:\n");
            execute_tokens(tokens, i + 1, end); // Right side
            return;
        }
    }

    // If we reach here, it means there are no more semicolons, pipes, or parentheses
    // Handle the remaining tokens (e.g., executing external commands)


    printf("Executing command:\n");
    // execute_command(tokens, start, end);
}



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

        // Check if we exited
        if (strcmp(input, "exit\n") == 0) {
            printf("Bye bye.\n");
            break;
        }

        tokens = tokenize(input);

        // for (int i = 0; *tokens[i].value != '\0'; i++) {
        //     printf("Token %d: %s, Type: %c\n", i, tokens[i].value, tokens[i].type);
        // }

        // Now you can process and execute the command using the tokens.

        execute_tokens(tokens, 0, sizeof(tokens) / MAX_INPUT_LENGTH);

        free(tokens);
    }

    return 0;
}

