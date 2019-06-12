#ifndef _LINUX_FDLOCK_H
#define _LINUX_FDLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

int fd_initlock(char* name, int rw);
int fd_trylock(int fd, int rw);
int fd_lock(int fd, int rw);
int fd_unlock(int fd, int rw);


#ifdef __cplusplus
}
#endif

#endif