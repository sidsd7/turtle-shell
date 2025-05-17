#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

void custom_rmdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rmdir: missing operand\n");
        return;
    }
    
    // Check if directory exists and is a directory
    struct stat st;
    if (stat(args[1], &st) == -1) {
        fprintf(stderr, "rmdir: failed to access '%s': %s\n", args[1], strerror(errno));
        return;
    }
    
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "rmdir: failed to remove '%s': Not a directory\n", args[1]);
        return;
    }
    
    // Check if directory is empty
    DIR *dir = opendir(args[1]);
    if (dir == NULL) {
        fprintf(stderr, "rmdir: failed to open '%s': %s\n", args[1], strerror(errno));
        return;
    }
    
    struct dirent *ent;
    int is_empty = 1;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            is_empty = 0;
            break;
        }
    }
    closedir(dir);
    
    if (!is_empty) {
        fprintf(stderr, "rmdir: failed to remove '%s': Directory not empty\n", args[1]);
        return;
    }
    
    if (rmdir(args[1]) == -1) {
        fprintf(stderr, "rmdir: failed to remove '%s': %s\n", args[1], strerror(errno));
    }
}
