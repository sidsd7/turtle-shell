#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <string.h>

void custom_ls_long(char **args) {
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
            continue;
        }
        
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir_name, entry->d_name);
        
        struct stat st;
        if (stat(fullpath, &st) == -1) {
            fprintf(stderr, "ls: cannot stat '%s': %s\n", fullpath, strerror(errno));
            continue;
        }
        
        // Print permissions
        printf("%c%c%c%c%c%c%c%c%c%c ",
               S_ISDIR(st.st_mode) ? 'd' : '-',
               st.st_mode & S_IRUSR ? 'r' : '-',
               st.st_mode & S_IWUSR ? 'w' : '-',
               st.st_mode & S_IXUSR ? 'x' : '-',
               st.st_mode & S_IRGRP ? 'r' : '-',
               st.st_mode & S_IWGRP ? 'w' : '-',
               st.st_mode & S_IXGRP ? 'x' : '-',
               st.st_mode & S_IROTH ? 'r' : '-',
               st.st_mode & S_IWOTH ? 'w' : '-',
               st.st_mode & S_IXOTH ? 'x' : '-');
        
        // Print number of links
        printf("%ld ", (long)st.st_nlink);
        
        // Print owner and group
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");
        
        // Print size
        printf("%8ld ", (long)st.st_size);
        
        // Print modification time
        char time_str[32];
        struct tm *tm = localtime(&st.st_mtime);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", tm);
        printf("%s ", time_str);
        
        // Print name
        printf("%s\n", entry->d_name);
    }
    
    closedir(dir);
}
