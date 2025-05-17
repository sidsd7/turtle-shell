#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

void custom_cat(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "cat: missing file operand\n");
        return;
    }
    
    // Check if file exists and is a regular file
    struct stat st;
    if (stat(args[1], &st) == -1) {
        fprintf(stderr, "cat: %s: %s\n", args[1], strerror(errno));
        return;
    }
    
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "cat: %s: Not a regular file\n", args[1]);
        return;
    }
    
    FILE *file = fopen(args[1], "r");
    if (file == NULL) {
        fprintf(stderr, "cat: %s: %s\n", args[1], strerror(errno));
        return;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL) {
        if (fputs(line, stdout) == EOF) {
            fprintf(stderr, "cat: write error\n");
            break;
        }
    }
    
    if (ferror(file)) {
        fprintf(stderr, "cat: %s: %s\n", args[1], strerror(errno));
    }
    
    fclose(file);
}
