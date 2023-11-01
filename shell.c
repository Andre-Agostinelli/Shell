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

Token* extract_tokens(Token* original_tokens, int start, int end) {
    int num_tokens = end - start + 1; // Calculate the number of tokens to extract

    Token* extracted_tokens = malloc((num_tokens + 1) * sizeof(Token)); // Allocate memory for extracted tokens
    if (extracted_tokens == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    for (int i = start; i <= end; i++) {
        extracted_tokens[i - start] = original_tokens[i]; // Copy tokens from original list to extracted list
    }

    extracted_tokens[num_tokens].value[0] = '\0'; // Add a sentinel token with an empty value to mark the end

    return extracted_tokens;
}

// Executes a pre-existing command using execvp
// Makes a child process so that we don't kill the shell
void execute_preexisting_command(Token* originalTokens, int start, int end) {

    Token* tokens = extract_tokens(originalTokens, start, end);

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

// finds the semicolon and returns the index
int find_sequencing(Token* tokens, int start, int end) {
    for (int i = start; i < end; i++) {
        if (strcmp(tokens[i].value, ";") == 0) {
            return i;
        }
    }
    return -1;
}

// counts all the elements in the array of tokens
int count_tokens(Token* tokens) {
    int count = 0;
    while (tokens[count].value[0] != '\0') {
        count++;
    }
    return count;
}

void execute_recursive(Token* tokens, int start, int end) {
    
    // char* input_file = NULL;
    // char* output_file = NULL;
    int seq_result = find_sequencing(tokens, start, end);

    // If a semicolon was found
    if (seq_result != -1) {
        // Create a child process to execute the first part of the sequence
        pid_t left_pid = fork();
        
        if (left_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (left_pid == 0) {
            // This is the child process (left part)
            execute_recursive(tokens, start, seq_result - 1);
            exit(0); // Child process exits
        } else {
            // This is the parent process
            int status;
            // Wait for the first child to finish
            waitpid(left_pid, &status, 0);
        }

        // Create a child process to execute the second part of the sequence
        pid_t right_pid = fork();
        
        if (right_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (right_pid == 0) {
            // This is the child process (right part)
            execute_recursive(tokens, seq_result + 1, end);
            exit(0); // Child process exits
        } else {
            // This is the parent process
            int status;
            // Wait for the second child to finish
            waitpid(right_pid, &status, 0);
        }
    }
    
    int i;
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


        // this is running again after both left and right children (BAD)
        execute_preexisting_command(tokens, start, end);
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

        execute_recursive(tokens, 0, count_tokens(tokens) - 1);
        free(tokens);
    }

    return 0;
}



