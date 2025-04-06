#define FUSE_USE_VERSION 31                                                                                                                                                                                     
#include <fuse3/fuse.h> 
#include <stdio.h>      
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>     
#include <sys/stat.h>   
#include <dirent.h>     
#include <sys/types.h>  
#include <sys/time.h>       // for struct timespec, utimensat
#include <sys/statvfs.h>    // for statvfs

#define MAX_PATH_LEN 1024

static const char* BACKING_DIR = "/home/moumou/ram_fs";

static void make_real_path(char *out_path, const char *path)
{   
    if (strlen(path) == 0) {
        snprintf(out_path, MAX_PATH_LEN, "%s", BACKING_DIR);
    } else {
        if (path[0] == '/') {
            snprintf(out_path, MAX_PATH_LEN, "%s%s", BACKING_DIR, path);
        } else {
            snprintf(out_path, MAX_PATH_LEN, "%s/%s", BACKING_DIR, path);
        }
    }
}

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &uc_oper, NULL);
}
