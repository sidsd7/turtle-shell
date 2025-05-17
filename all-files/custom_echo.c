#include <stdio.h>
#include <string.h>
#include "../shell.h"  // Add this to get the MAX_LINE definition

void custom_echo(char **args) {
    int i = 1;
    int newline = 1;
    
    // Check for -n option
    if (args[i] != NULL && strcmp(args[i], "-n") == 0) {
        newline = 0;
        i++;
    }
    
    // Print all arguments with spaces between them
    int first = 1;
    while (args[i] != NULL) {
        // Add space between arguments (but not before the first one)
        if (!first) {
            printf(" ");
        }
        first = 0;
        
        // Handle quotes
        char *text = args[i];
        int len = strlen(text);
        
        // Check if the argument is quoted and remove quotes if it is
        if (len >= 2 && (
            (text[0] == '"' && text[len-1] == '"') || 
            (text[0] == '\'' && text[len-1] == '\'')
        )) {
            // Print without the quotes
            text[len-1] = '\0';  // Terminate string before the closing quote
            printf("%s", text + 1);  // Print starting after the opening quote
        } else {
            printf("%s", text);
        }
        
        i++;
    }
    
    if (newline) {
        printf("\n");
    }
}
