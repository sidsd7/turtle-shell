#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

// Constants
#define MAX_HISTORY 100
#define MAX_ALIASES 50
#define MAX_ARGS 64
#define MAX_LINE 1024

// Structures
typedef struct {
    char commands[MAX_HISTORY][MAX_LINE];
    int count;
} CommandHistory;

typedef struct {
    char name[50];
    char command[MAX_LINE];
} Alias;

typedef struct {
    const char *name;
    const char *description;
    void (*func)(char **);
    const char *usage;
} Command;

typedef struct {
    char **tokens;
    int token_count;
    char *raw_input;
} ParsedCommand;

// Global variables
extern CommandHistory history;
extern Alias aliases[MAX_ALIASES];
extern int alias_count;

// Command functions
void custom_ls(char **args);
void custom_ls_long(char **args);
void custom_cat(char **args);
void custom_cp(char **args);
void custom_mkdir(char **args);
void custom_rmdir(char **args);
void custom_rm(char **args);
void custom_echo(char **args);
void custom_touch(char **args);
void custom_cd(char **args);
void custom_pwd(char **args);
void custom_history(char **args);
void custom_alias(char **args);
void custom_unalias(char **args);
void custom_help(char **args);

// Shell functions
void init_shell(void);
void add_to_history(const char *command);
char* check_alias(const char *command);
void executeCommand(char **args);
void shellLoop(void);
void setup_signals(void);
void parse_shell_args(int argc, char **argv);
char* complete_command(const char *input);

ParsedCommand* parse_command(const char *input);
void free_parsed_command(ParsedCommand *cmd);

// AI integration functions
void process_natural_language_input(const char *input);
void validate_and_execute_command(const char *command);

static const Command commands[] = {
    {"custom_ls", "List directory contents", custom_ls, "custom_ls [directory]"},
    {"custom_ls_long", "List directory contents in long format", custom_ls_long, "custom_ls_long [directory]"},
    {"custom_cat", "Display file contents", custom_cat, "custom_cat <file>"},
    {"custom_cp", "Copy file", custom_cp, "custom_cp <src> <dst>"},
    {"custom_mkdir", "Create directory", custom_mkdir, "custom_mkdir <dir>"},
    {"custom_rmdir", "Remove empty directory", custom_rmdir, "custom_rmdir <dir>"},
    {"custom_rm", "Remove file", custom_rm, "custom_rm <file>"},
    {"custom_echo", "Display text", custom_echo, "custom_echo [text]"},
    {"custom_touch", "Create empty file", custom_touch, "custom_touch <file>"},
    {"custom_cd", "Change directory", custom_cd, "custom_cd [directory]"},
    {"custom_pwd", "Print working directory", custom_pwd, "custom_pwd"},
    {"custom_history", "Show command history", custom_history, "custom_history"},
    {"custom_alias", "Create or list aliases", custom_alias, "custom_alias [name=cmd]"},
    {"custom_unalias", "Remove alias", custom_unalias, "custom_unalias <name>"},
    {"custom_help", "Show help information", custom_help, "custom_help"},
    {"custom_exit", "Exit the shell", NULL, "custom_exit"},
    {NULL, NULL, NULL, NULL}
};

#define SHELL_ERROR(msg, ...) \
    fprintf(stderr, "Error: " msg "\n", ##__VA_ARGS__)

#define SHELL_COMMAND_ERROR(cmd, msg, ...) \
    fprintf(stderr, "%s: " msg "\n", cmd, ##__VA_ARGS__)

#endif // SHELL_H 