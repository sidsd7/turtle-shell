#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <utime.h>
#include <time.h>
#include "../shell.h"

void custom_touch(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "touch: missing file operand\n");
        return;
    }
    
    struct stat st;
    if (stat(args[1], &st) == 0) {
        struct utimbuf new_times;
        new_times.actime = new_times.modtime = time(NULL);
        
        if (utime(args[1], &new_times) == -1) {
            fprintf(stderr, "touch: cannot touch '%s': %s\n", 
                           args[1], strerror(errno));
        }
    } else {
        int fd = open(args[1], O_CREAT | O_WRONLY, 0644);
        if (fd == -1) {
            fprintf(stderr, "touch: cannot create '%s': %s\n", 
                           args[1], strerror(errno));
            return;
        }
        close(fd);
    }
}
