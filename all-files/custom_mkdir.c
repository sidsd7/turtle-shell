#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

void custom_mkdir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "mkdir: missing operand\n");
        return;
    }
    
    // Check if directory already exists
    struct stat st;
    if (stat(args[1], &st) == 0) {
        fprintf(stderr, "mkdir: cannot create directory '%s': File exists\n", args[1]);
        return;
    }
    
    // Create directory with default permissions (0755)
    if (mkdir(args[1], 0755) == -1) {
        fprintf(stderr, "mkdir: cannot create directory '%s': %s\n", args[1], strerror(errno));
        return;
    }
}
