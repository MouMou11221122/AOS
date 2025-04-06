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

/* create */
static int r_ramfs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{   
    char real_path[MAX_PATH_LEN];
    make_real_path(real_path, path);

    /* TODO: implement remote create using rdma */
    int fd = open(real_path, fi->flags | O_CREAT, mode);

    if (fd == -1) {
        return -errno;
    }
    fi->fh = fd;
    return 0;
}

/* open */
static int r_ramfs_open(const char *path, struct fuse_file_info *fi)
{
    char real_path[MAX_PATH_LEN];
    make_real_path(real_path, path);

    /* TODO: implement remote open using rdma */
    int fd = open(real_path, fi->flags);

    if (fd == -1) {
        return -errno;                                                                                                                                                                                          
    }
    fi->fh = fd;
    return 0;
}

/* getattr */
static int r_ramfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
    (void) fi;
    char real_path[MAX_PATH_LEN];
    memset(stbuf, 0, sizeof(struct stat));
    make_real_path(real_path, path);

    /* TODO: implement remote getattr using rdma */
    if (lstat(real_path, stbuf) == -1) {
        return -errno;
    }

    return 0;
}

/* readdir */
static int r_ramfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                      off_t offset, struct fuse_file_info *fi,
                      enum fuse_readdir_flags flags)
{                 
    (void) offset;
    (void) fi;    
    (void) flags; 
    char real_path[MAX_PATH_LEN];
    make_real_path(real_path, path);

    /* TODO: implement remote readdir using rdma */
    DIR *dp = opendir(real_path);
    if (!dp) {    
        return -errno;
    }             
    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {                                                                                                                                                                        
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino  = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0, (enum fuse_fill_dir_flags)0) != 0) {
            break;
        }         
    }             
    closedir(dp);

    return 0;     
} 

/* mkdir */
static int r_ramfs_mkdir(const char *path, mode_t mode)
{
    char real_path[MAX_PATH_LEN];
    make_real_path(real_path, path);

    /* TODO: implement remote mkdir using rdma */
    if (mkdir(real_path, mode) == -1) {
        return -errno;
    }

    return 0;
}

/* fuse_operations struct */
static struct fuse_operations uc_oper = {
    .create   = r_ramfs_create,
    .open     = r_ramfs_open,
    .getattr  = r_ramfs_getattr,
    .readdir  = r_ramfs_readdir,
    .mkdir    = r_ramfs_mkdir,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &uc_oper, NULL);
}
