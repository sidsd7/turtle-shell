#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

void custom_rm(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "rm: missing operand\n");
        return;
    }
    
    // Check if file exists and is a regular file
    struct stat st;
    if (stat(args[1], &st) == -1) {
        fprintf(stderr, "rm: cannot remove '%s': %s\n", args[1], strerror(errno));
        return;
    }
    
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "rm: cannot remove '%s': Not a regular file\n", args[1]);
        return;
    }
    
    if (unlink(args[1]) == -1) {
        fprintf(stderr, "rm: cannot remove '%s': %s\n", args[1], strerror(errno));
    }
}
