#include "../include/file_lock.h"
#include <stdio.h>

int set_read_lock(int fd, int start, int len) {
    struct flock lock;
    
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;
    
    return fcntl(fd, F_SETLKW, &lock);
}

int set_write_lock(int fd, int start, int len) {
    struct flock lock;
    
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;
    
    return fcntl(fd, F_SETLKW, &lock);
}

int release_lock(int fd, int start, int len) {
    struct flock lock;
    
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;
    
    return fcntl(fd, F_SETLKW, &lock);
}