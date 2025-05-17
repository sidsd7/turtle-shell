#include "shell.h"
#include <termios.h>
#include "include/ai_integration.h"

// Global variables
CommandHistory history;
Alias aliases[MAX_ALIASES];
int alias_count = 0;
int history_position = -1;  // For history navigation
char current_input[MAX_LINE] = "";  // For command editing
struct termios orig_term;  // Store original terminal settings

// Signal handler for Ctrl+C
void sigint_handler(int sig) {
    (void)sig; // Unused parameter
    printf("\n");
    fflush(stdout);
}

// Print shell name and decoration
void print_shell_name(void) {
    printf("\033[1;32m");  // Green color
    printf("  _______           _   _      \n");
    printf(" |__   __|         | | | |     \n");
    printf("    | |_   _ _ __  | |_| | ___ \n");
    printf("    | | | | | '__| | __| |/ _ \\\n");
    printf("    | | |_| | |    | |_| |  __/\n");
    printf("    |_|\\__,_|_|     \\__|_|\\___|\n");
    printf("\033[0m");  // Reset color
    printf("Welcome to Turtle Shell!\n");
    printf("Type 'custom_help' for available commands.\n\n");
}

// Initialize shell
void init_shell(void) {
    // Initialize history
    history.count = 0;
    history_position = -1;
    
    // Set up signal handlers
    setup_signals();
    
    // Add default aliases
    strcpy(aliases[alias_count].name, "ll");
    strcpy(aliases[alias_count].command, "custom_ls_long");
    alias_count++;
    
    strcpy(aliases[alias_count].name, "exit");
    strcpy(aliases[alias_count].command, "custom_exit");
    alias_count++;
    
    // Print shell name
    print_shell_name();
}

// Add command to history
void add_to_history(const char *command) {
    if (strlen(command) == 0) return;  // Don't add empty commands
    
    if (history.count >= MAX_HISTORY) {
        // Shift all commands up, free the oldest one
        memmove(history.commands[0], history.commands[1], 
                (MAX_HISTORY - 1) * sizeof(history.commands[0]));
        history.count--;
    }
    
    strncpy(history.commands[history.count], command, MAX_LINE - 1);
    history.commands[history.count][MAX_LINE - 1] = '\0';
    history.count++;
    history_position = -1;  // Reset history position
}

// Check if command is an alias
char* check_alias(const char *command) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(command, aliases[i].name) == 0) {
            return aliases[i].command;
        }
    }
    return NULL;
}

// Command completion
char* complete_command(const char *input) {
    static char completion[MAX_LINE];
    int input_len = strlen(input);
    
    // Check for command completion
    const char *commands[] = {
        "custom_ls", "custom_ls_long", "custom_cat", "custom_cp",
        "custom_mkdir", "custom_rmdir", "custom_rm", "custom_echo",
        "custom_touch", "custom_cd", "custom_pwd", "custom_history",
        "custom_alias", "custom_unalias", "custom_help", "custom_exit",
        NULL
    };
    
    for (int i = 0; commands[i] != NULL; i++) {
        if (strncmp(input, commands[i], input_len) == 0) {
            strcpy(completion, commands[i]);
            return completion;
        }
    }
    
    // Check for alias completion
    for (int i = 0; i < alias_count; i++) {
        if (strncmp(input, aliases[i].name, input_len) == 0) {
            strcpy(completion, aliases[i].name);
            return completion;
        }
    }
    
    return NULL;
}

void executeCommand(char **args) {
    if (args[0] == NULL) return;
    
    // Add this line at the beginning of the function
    printf("\n");  // Print a newline to separate prompt from command output
    
    // Check for aliases
    char *alias_command = check_alias(args[0]);
    if (alias_command) {
        // Make a copy of the alias command
        char alias_copy[MAX_LINE];
        strncpy(alias_copy, alias_command, MAX_LINE-1);
        alias_copy[MAX_LINE-1] = '\0';
        
        // Create a static array to hold the expanded alias command
        static char *alias_args[MAX_ARGS];
        
        // Parse the alias command
        char *token = strtok(alias_copy, " ");
        int i = 0;
        while (token != NULL && i < MAX_ARGS - 1) {
            alias_args[i++] = token;
            token = strtok(NULL, " ");
        }
        alias_args[i] = NULL;
        
        // Combine with any additional arguments
        if (args[1] != NULL) {
            int j = 1;
            while (args[j] != NULL && i < MAX_ARGS - 1) {
                alias_args[i++] = args[j++];
            }
            alias_args[i] = NULL;
        }
        
        // Use the alias_args for execution
        if (strcmp(alias_args[0], "custom_exit") == 0) {
            struct termios term;
            tcgetattr(STDIN_FILENO, &term);
            term.c_lflag |= (ECHO | ICANON);  // Restore normal terminal mode
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
            exit(0);
        } else if (strcmp(alias_args[0], "custom_cd") == 0) {
            custom_cd(alias_args);
        } else if (strcmp(alias_args[0], "custom_pwd") == 0) {
            custom_pwd(alias_args);
        } else if (strcmp(alias_args[0], "custom_history") == 0) {
            custom_history(alias_args);
        } else if (strcmp(alias_args[0], "custom_alias") == 0) {
            custom_alias(alias_args);
        } else if (strcmp(alias_args[0], "custom_unalias") == 0) {
            custom_unalias(alias_args);
        } else if (strcmp(alias_args[0], "custom_help") == 0) {
            custom_help(alias_args);
        } else if (strcmp(alias_args[0], "custom_ls") == 0) {
            custom_ls(alias_args);
        } else if (strcmp(alias_args[0], "custom_ls_long") == 0) {
            custom_ls_long(alias_args);
        } else if (strcmp(alias_args[0], "custom_cat") == 0) {
            custom_cat(alias_args);
        } else if (strcmp(alias_args[0], "custom_cp") == 0) {
            custom_cp(alias_args);
        } else if (strcmp(alias_args[0], "custom_mkdir") == 0) {
            custom_mkdir(alias_args);
        } else if (strcmp(alias_args[0], "custom_rmdir") == 0) {
            custom_rmdir(alias_args);
        } else if (strcmp(alias_args[0], "custom_rm") == 0) {
            custom_rm(alias_args);
        } else if (strcmp(alias_args[0], "custom_echo") == 0) {
            custom_echo(alias_args);
        } else if (strcmp(alias_args[0], "custom_touch") == 0) {
            custom_touch(alias_args);
        } else {
            printf("Unknown command: %s\n", alias_args[0]);
        }
        fflush(stdout);
        return;
    }
    
    // Non-alias commands
    if (strcmp(args[0], "custom_exit") == 0) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
        exit(0);
    } else if (strcmp(args[0], "custom_cd") == 0) {
        custom_cd(args);
    } else if (strcmp(args[0], "custom_pwd") == 0) {
        custom_pwd(args);
    } else if (strcmp(args[0], "custom_history") == 0) {
        custom_history(args);
    } else if (strcmp(args[0], "custom_alias") == 0) {
        custom_alias(args);
    } else if (strcmp(args[0], "custom_unalias") == 0) {
        custom_unalias(args);
    } else if (strcmp(args[0], "custom_help") == 0) {
        custom_help(args);
    } else if (strcmp(args[0], "custom_ls") == 0) {
        custom_ls(args);
    } else if (strcmp(args[0], "custom_ls_long") == 0) {
        custom_ls_long(args);
    } else if (strcmp(args[0], "custom_cat") == 0) {
        custom_cat(args);
    } else if (strcmp(args[0], "custom_cp") == 0) {
        custom_cp(args);
    } else if (strcmp(args[0], "custom_mkdir") == 0) {
        custom_mkdir(args);
    } else if (strcmp(args[0], "custom_rmdir") == 0) {
        custom_rmdir(args);
    } else if (strcmp(args[0], "custom_rm") == 0) {
        custom_rm(args);
    } else if (strcmp(args[0], "custom_echo") == 0) {
        custom_echo(args);
    } else if (strcmp(args[0], "custom_touch") == 0) {
        custom_touch(args);
    } else {
        printf("Unknown command: %s\n", args[0]);
    }
    fflush(stdout);
}

void shellLoop(void) {
    char inputLine[MAX_LINE];
    char cwd[MAX_LINE];
    int c;
    int position;

    init_shell();

    // Set terminal to raw mode to handle input manually
    struct termios term, term_orig;
    tcgetattr(STDIN_FILENO, &term_orig); // Save original settings
    term = term_orig;
    term.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
    term.c_cc[VMIN] = 1;  // Read one char at a time
    term.c_cc[VTIME] = 0; // No timeout
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    // Add after terminal setup
    init_ai_integration();

    while (1) {
        // Ensure stdout is flushed before displaying the prompt
        fflush(stdout);
        
        // Get current working directory for prompt
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("\033[1;32m%s\033[0m> ", cwd);  // No newline
        } else {
            printf("> ");  // No newline
        }
        fflush(stdout);  // Ensure prompt is displayed
        
        // Reset position for new input
        position = 0;
        memset(inputLine, 0, MAX_LINE);
        
        // Read input character by character
        while ((c = getchar()) != '\n' && c != EOF) {
            if (c == '\t') {  // Tab completion
                inputLine[position] = '\0';
                char *completion = complete_command(inputLine);
                if (completion) {
                    printf("\r\033[K");  // Clear line
                    printf("\033[1;32m%s\033[0m> %s", cwd, completion);
                    strcpy(inputLine, completion);
                    position = strlen(completion);
                }
            } else if (c == 127 || c == 8) {  // Backspace
                if (position > 0) {
                    position--;
                    printf("\b \b");
                }
            } else {
                // Check for buffer overflow
                if (position >= MAX_LINE - 1) {
                    fprintf(stderr, "Command too long\n");
                    position = 0;
                    while ((c = getchar()) != '\n' && c != EOF); // Clear remaining input
                    break;
                }
                
                inputLine[position++] = c;
                putchar(c); // Echo the character manually
            }
        }
        
        // Restore terminal settings when exiting the loop
        if (c == EOF) {
            tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
            printf("\n");
            break;
        }
        
        inputLine[position] = '\0';
        
        // Skip empty lines
        if (strlen(inputLine) == 0) continue;
        
        // Add to history
        add_to_history(inputLine);

        // After getting input but before parsing:
        if (inputLine[0] == '!') {
            // Natural language mode
            process_natural_language_input(inputLine + 1);
            continue;
        }
        
        // Before executing command:
        validate_and_execute_command(inputLine);
    }

    // Add before terminal cleanup
    cleanup_ai_integration();

    // Ensure we restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);
}

void setup_signals(void) {
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    
    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
}

void parse_shell_args(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  --help     Show this help message\n");
            exit(0);
        }
        // Add more command line options as needed
    }
}

// Implementation for parse_command
ParsedCommand* parse_command(const char *input) {
    ParsedCommand *cmd = malloc(sizeof(ParsedCommand));
    if (!cmd) {
        SHELL_ERROR("Memory allocation failed");
        return NULL;
    }
    
    // Make a copy of the input for raw_input
    cmd->raw_input = strdup(input);
    if (!cmd->raw_input) {
        free(cmd);
        SHELL_ERROR("Memory allocation failed");
        return NULL;
    }
    
    // Count the number of tokens
    int token_count = 0;
    char *input_copy = strdup(input);
    if (!input_copy) {
        free(cmd->raw_input);
        free(cmd);
        SHELL_ERROR("Memory allocation failed");
        return NULL;
    }
    
    char *token = strtok(input_copy, " \t\n");
    while (token != NULL) {
        token_count++;
        token = strtok(NULL, " \t\n");
    }
    free(input_copy);
    
    // Allocate memory for tokens
    cmd->tokens = malloc((token_count + 1) * sizeof(char *));
    if (!cmd->tokens) {
        free(cmd->raw_input);
        free(cmd);
        SHELL_ERROR("Memory allocation failed");
        return NULL;
    }
    
    // Copy tokens
    input_copy = strdup(input);
    if (!input_copy) {
        free(cmd->tokens);
        free(cmd->raw_input);
        free(cmd);
        SHELL_ERROR("Memory allocation failed");
        return NULL;
    }
    
    token = strtok(input_copy, " \t\n");
    int i = 0;
    while (token != NULL && i < token_count) {
        cmd->tokens[i] = strdup(token);
        if (!cmd->tokens[i]) {
            for (int j = 0; j < i; j++) {
                free(cmd->tokens[j]);
            }
            free(cmd->tokens);
            free(cmd->raw_input);
            free(cmd);
            free(input_copy);
            SHELL_ERROR("Memory allocation failed");
            return NULL;
        }
        i++;
        token = strtok(NULL, " \t\n");
    }
    cmd->tokens[i] = NULL;
    cmd->token_count = i;
    
    free(input_copy);
    return cmd;
}

void free_parsed_command(ParsedCommand *cmd) {
    if (cmd) {
        if (cmd->tokens) {
            for (int i = 0; i < cmd->token_count; i++) {
                free(cmd->tokens[i]);
            }
            free(cmd->tokens);
        }
        free(cmd->raw_input);
        free(cmd);
    }
}

void handle_redirection(ParsedCommand *cmd) {
    (void)cmd; // Suppress unused parameter warning
    // Implementation for handling I/O redirection
}

void handle_pipeline(ParsedCommand *cmd) {
    (void)cmd; // Suppress unused parameter warning
    // Implementation for command pipelines
}

void custom_setenv(char **args) {
    (void)args; // Suppress unused parameter warning
    // Implementation for setting environment variables
}

void custom_printenv(char **args) {
    (void)args; // Suppress unused parameter warning
    // Implementation for printing environment variables
}

void time_command_execution(void (*cmd_func)(char **), char **args) {
    (void)cmd_func; // Suppress unused parameter warning
    (void)args; // Suppress unused parameter warning
    // Implementation for timing command execution
}

// Add this function to disable terminal echo
void disable_terminal_echo(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

// And this to restore echo
void enable_terminal_echo(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

void process_natural_language_input(const char *input) {
    char *command = natural_language_to_command(input);
    if (command) {
        // Remove any newlines from the response
        char *newline = strchr(command, '\n');
        if (newline) *newline = '\0';
        
        printf("Translated command: %s\n", command);
        printf("Execute? [y/N]: ");
        fflush(stdout);
        
        char response;
        scanf(" %c", &response);
        if (response == 'y' || response == 'Y') {
            validate_and_execute_command(command);
        }
        free(command);
    } else {
        printf("Failed to translate natural language input\n");
    }
}

void validate_and_execute_command(const char *command) {
    char *validation = validate_command(command);
    if (validation) {
        // Remove any newlines from the response
        char *newline = strchr(validation, '\n');
        if (newline) *newline = '\0';
        
        printf("AI Validation: %s\n", validation);
        
        // Add this check for unknown commands
        if (strstr(validation, "Unknown command") != NULL) {
            // Don't even ask to proceed with unknown commands
            free(validation);
            return;  // Exit without executing the command
        }
        
        if (strstr(validation, "Command is safe") == NULL) {
            printf("Proceed? [y/N]: ");
            fflush(stdout);
            
            char response;
            scanf(" %c", &response);
            if (response != 'y' && response != 'Y') {
                free(validation);
                return;
            }
        }
        
        // Parse and execute command
        char *args[MAX_ARGS];
        char cmd_copy[MAX_LINE];
        strncpy(cmd_copy, command, MAX_LINE-1);
        
        char *token = strtok(cmd_copy, " ");
        int position = 0;
        while (token != NULL && position < MAX_ARGS - 1) {
            args[position++] = token;
            token = strtok(NULL, " ");
        }
        args[position] = NULL;
        
        executeCommand(args);
        free(validation);
    }
}

int main(int argc, char **argv) {
    tcgetattr(STDIN_FILENO, &orig_term);  // Save original terminal settings
    
    parse_shell_args(argc, argv);
    shellLoop();
    
    // Make sure we restore terminal settings before exiting
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
    return 0;
} 