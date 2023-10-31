#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

// Handles the cd command
void cd_command(char *directory) {
    if (chdir(directory) != 0) {
        perror("cd");
    }
}

// Handles the help command
void help_command() {
    printf("\'cd\': Change current directory.\n");
    printf("\'source\': Execute commands from a file.\n");
    printf("\'prev\': Repeat the previous command.\n");
    printf("\'help\': Display help for built-in commands.\n");
}

// // Given a file name, returns a list of tokens that can be used to execute  
// Token* source_command(char* filename) {
//     FILE* file = fopen(filename, "r");

//     if (file == NULL) {
//         perror("Error opening file");
//         return NULL;
//     }

//     char line[MAX_INPUT_LENGTH];
//     Token* tokens = NULL;

//     while (fgets(line, MAX_INPUT_LENGTH, file) != NULL) {
//         printf("Executing: %s", line);

//         // Tokenize the line and add tokens to the list
//         Token* line_tokens = tokenize(line);

//         if (tokens == NULL) {
//             tokens = line_tokens;
//         } else {
//             tokens = realloc(tokens, (count_tokens(tokens) + count_tokens(line_tokens)) * sizeof(Token));
//             if (tokens == NULL) {
//                 perror("Error allocating memory");
//                 exit(EXIT_FAILURE);
//             }

//             int i = count_tokens(tokens);
//             int j = 0;

//             while (line_tokens[j].value[0] != '\0') {
//                 tokens[i++] = line_tokens[j++];
//             }

//             free(line_tokens);
//         }
//     }

//     fclose(file);
//     return tokens;
// }


// Executes a pre-existing command using execvp
// Makes a child process so that we don't kill the shell
void execute_preexisting_command(Token* tokens) {
    pid_t pid = fork();

    if (pid == 0) {
        // This is the child process
        int i = 0;
        char* args[MAX_INPUT_LENGTH]; // Assuming a maximum length for arguments

        while (tokens[i].type == 'W' && tokens[i].value[0] != '\0') {
            args[i] = tokens[i].value;
            i++;
        }

        args[i] = NULL; // Make sure the last element is NULL for execvp

        if (execvp(args[0], args) == -1) {
            perror("Error");
            _exit(1); // Make sure to exit child on execvp failure
        }
    } else if (pid > 0) {
        // This is the parent process
        int status;
        wait(&status); // Wait for the child process to finish
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) != 0) {
                printf("Child process exited with non-zero status %d\n", WEXITSTATUS(status));
            }
        } else {
            printf("Child process did not exit normally\n");
        }
    } else {
        perror("fork");
    }
}


void execute_recursive(Token* tokens, int start, int end) {

    int i;
    // char* input_file = NULL;
    // char* output_file = NULL;
    
    for (i = start; i <= end; i++) {

        if (strcmp(tokens[i].value, "cd") == 0) { 
            cd_command(tokens[i + 1].value);
            return; 
        } else if (strcmp(tokens[i].value, "source") == 0) {
            // if (tokens[i + 1].type == 'W') {
            //     Token* tokens_from_source = source_command(tokens[i + 1].value);
            //     execute_recursive(tokens_from_source, 0, end);
            //     free(tokens_from_source);
            // } else {
            //     printf("Usage: source <filename>\n");
            // }
            printf("Need to handle \'source\'\n");
            return; 
        } else if (strcmp(tokens[i].value, "prev") == 0) {
            printf("Need to handle \'prev\'\n");
            return; 
        } else if (strcmp(tokens[i].value, "help") == 0) {
            help_command(); 
            return; 
        }

        // Handle pre-existing commands using execvp
        execute_preexisting_command(tokens);
        return; 
    }
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

        execute_recursive(tokens, 0, sizeof(tokens) / MAX_INPUT_LENGTH);
        free(tokens);
    }

    return 0;
}


