#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

void custom_cp(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "cp: missing file operand\n");
        return;
    }
    
    // Check if source file exists
    struct stat st;
    if (stat(args[1], &st) == -1) {
        fprintf(stderr, "cp: cannot stat '%s': %s\n", args[1], strerror(errno));
        return;
    }
    
    // Check if source is a regular file
    if (!S_ISREG(st.st_mode)) {
        fprintf(stderr, "cp: '%s' is not a regular file\n", args[1]);
        return;
    }
    
    // Open source file
    int src_fd = open(args[1], O_RDONLY);
    if (src_fd == -1) {
        fprintf(stderr, "cp: cannot open '%s': %s\n", args[1], strerror(errno));
        return;
    }
    
    // Create destination file
    int dst_fd = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
    if (dst_fd == -1) {
        fprintf(stderr, "cp: cannot create '%s': %s\n", args[2], strerror(errno));
        close(src_fd);
        return;
    }
    
    // Copy file contents
    char buf[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buf, sizeof(buf))) > 0) {
        if (write(dst_fd, buf, bytes_read) != bytes_read) {
            fprintf(stderr, "cp: error writing to '%s': %s\n", args[2], strerror(errno));
            break;
        }
    }
    
    if (bytes_read == -1) {
        fprintf(stderr, "cp: error reading from '%s': %s\n", args[1], strerror(errno));
    }
    
    close(src_fd);
    close(dst_fd);
}
