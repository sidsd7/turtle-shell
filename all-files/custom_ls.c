#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include "../shell.h"

void custom_ls(char **args) {
    char *dir_name = ".";
    if (args[1] != NULL) {
        dir_name = args[1];
    }
    
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", dir_name, strerror(errno));
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;  // Skip hidden files
        }
        printf("%s\n", entry->d_name);
    }
    
    closedir(dir);
} 