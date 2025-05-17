#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

#include "shell_types.h"

// Command function declarations
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

#endif 