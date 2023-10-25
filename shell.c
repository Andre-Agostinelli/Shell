#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LENGTH 1024

// Define a struct to hold tokens
typedef struct {
    char type; // 'W' for word, 'S' for special character
    char value[MAX_INPUT_LENGTH];
} Token;

// Function to check if a character is a special token
int is_special(char ch) {
    return (ch == '(' || ch == ')' || ch == '<' || ch == '>' || ch == ';' || ch == '|');
}

// Function to tokenize the input into a list of tokens
Token* tokenize(const char* input) {
    Token* tokens = malloc(MAX_INPUT_LENGTH * sizeof(Token));
    int tokenCount = 0;
    int i = 0;

    while (input[i] != '\0') {
        if (is_special(input[i])) {
            tokens[tokenCount].type = 'S';
            tokens[tokenCount].value[0] = input[i];
            tokens[tokenCount].value[1] = '\0';
            tokenCount++;
            i++;
        } else if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n') {
            // Skip whitespace
            i++;
        } else if (input[i] == '"') {
            i++; // Skip opening quote
            int j = 0;
            while (input[i] != '"' && input[i] != '\0') {
                tokens[tokenCount].type = 'W';
                tokens[tokenCount].value[j] = input[i];
                j++;
                i++;
            }
            tokens[tokenCount].value[j] = '\0'; // Null-terminate the token value
            tokenCount++;
            i++; // Skip closing quote
        } else {
            int j = 0;
            while (!is_special(input[i]) && input[i] != ' ' && input[i] != '\t' && input[i] != '\n' && input[i] != '\0') {
                tokens[tokenCount].type = 'W';
                tokens[tokenCount].value[j] = input[i];
                j++;
                i++;
            }
            tokens[tokenCount].value[j] = '\0'; // Null-terminate the token value
            tokenCount++;
        }
    }

    tokens[tokenCount].value[0] = '\0'; // Null-terminate the token array
    return tokens;
}

int main(int argc, char **argv) {
    char input[MAX_INPUT_LENGTH];
    Token* tokens;

    printf("Welcome to mini-shell.\n");

    while (1) {
        printf("shell $ ");
        fgets(input, MAX_INPUT_LENGTH, stdin);

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