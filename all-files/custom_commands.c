#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include "../shell.h"
#include <errno.h>

// External declarations
extern CommandHistory history;
extern Alias aliases[];
extern int alias_count;

// Aliases
#define MAX_ALIASES 50

// Custom cd implementation
void custom_cd(char **args) {
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

// Custom pwd implementation
void custom_pwd(char **args) {
    (void)args; // Unused parameter
    char cwd[MAX_LINE];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

// Custom history implementation
void custom_history(char **args) {
    (void)args; // Unused parameter
    
    for (int i = 0; i < history.count; i++) {
        printf("%d: %s\n", i + 1, history.commands[i]);
    }
}

// Custom alias implementation
void custom_alias(char **args) {
    if (args[1] == NULL) {
        // List all aliases
        for (int i = 0; i < alias_count; i++) {
            printf("%s='%s'\n", aliases[i].name, aliases[i].command);
        }
        return;
    }
    
    if (args[2] == NULL) {
        fprintf(stderr, "alias: missing command\n");
        return;
    }
    
    if (alias_count >= MAX_ALIASES) {
        fprintf(stderr, "alias: too many aliases\n");
        return;
    }
    
    // Check if alias already exists
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, args[1]) == 0) {
            strncpy(aliases[i].command, args[2], sizeof(aliases[i].command) - 1);
            aliases[i].command[sizeof(aliases[i].command) - 1] = '\0';
            return;
        }
    }
    
    // Add new alias
    strncpy(aliases[alias_count].name, args[1], sizeof(aliases[alias_count].name) - 1);
    aliases[alias_count].name[sizeof(aliases[alias_count].name) - 1] = '\0';
    strncpy(aliases[alias_count].command, args[2], sizeof(aliases[alias_count].command) - 1);
    aliases[alias_count].command[sizeof(aliases[alias_count].command) - 1] = '\0';
    alias_count++;
}

// Custom unalias implementation
void custom_unalias(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "unalias: missing alias name\n");
        return;
    }
    
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, args[1]) == 0) {
            // Shift remaining aliases
            for (int j = i; j < alias_count - 1; j++) {
                strcpy(aliases[j].name, aliases[j + 1].name);
                strcpy(aliases[j].command, aliases[j + 1].command);
            }
            alias_count--;
            return;
        }
    }
    
    fprintf(stderr, "unalias: %s: not found\n", args[1]);
}

// Custom help implementation
void custom_help(char **args) {
    (void)args; // Unused parameter
    
    printf("Available commands:\n");
    printf("  custom_ls [directory]    - List directory contents\n");
    printf("  custom_ls_long [dir]     - List directory contents in long format\n");
    printf("  custom_cat <file>        - Display file contents\n");
    printf("  custom_cp <src> <dst>    - Copy file\n");
    printf("  custom_mkdir <dir>       - Create directory\n");
    printf("  custom_rmdir <dir>       - Remove empty directory\n");
    printf("  custom_rm <file>         - Remove file\n");
    printf("  custom_echo [text]       - Display text\n");
    printf("  custom_touch <file>      - Create empty file\n");
    printf("  custom_cd [directory]    - Change directory\n");
    printf("  custom_pwd               - Print working directory\n");
    printf("  custom_history           - Show command history\n");
    printf("  custom_alias [name cmd]  - Create or list aliases\n");
    printf("  custom_unalias <name>    - Remove alias\n");
    printf("  custom_help              - Show this help message\n");
    printf("  custom_exit              - Exit the shell\n");
    printf("\nAliases:\n");
    for (int i = 0; i < alias_count; i++) {
        printf("  %s='%s'\n", aliases[i].name, aliases[i].command);
    }
} 