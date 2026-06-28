#ifndef FILE_LOCK_H
#define FILE_LOCK_H

#include <fcntl.h>
#include <unistd.h>

// Set a read lock on a file
int set_read_lock(int fd, int start, int len);

// Set a write lock on a file
int set_write_lock(int fd, int start, int len);

// Release a lock (read or write) on a file
int release_lock(int fd, int start, int len);

#endif