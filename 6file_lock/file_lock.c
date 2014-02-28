/*
  Description: the use of write lock.
    When one process A hold the write lock of file, then
    other process B want to set the write lock to file, it fails
	2.   FileName: write_lock.c
*/
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
        struct flock    lock;

        lock.l_type = type;     /* F_RDLCK, F_WRLCK, F_UNLCK */
        lock.l_start = offset;  /* byte offset, relative to l_whence */
        lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
        lock.l_len = len;       /* #bytes (0 means to EOF) */

        return( fcntl(fd, cmd, &lock) );
}

#define read_lock(fd, offset, whence, len) \
                        lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len)
#define write_lock(fd, offset, whence, len) \
                        lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len)
#define err_sys(x) { printf(x); exit(1); }
int main(int argc, char *argv[])
{
    int             fd;
    pid_t           pid;
    if (argc != 2) {
        fprintf(stderr, "usage: %s filename\n", argv[0]);
        exit(1);
    }
    if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC )) < 0)
        err_sys("open error");
    if (write(fd, "hello world", 11) != 11)
        err_sys("write error");

    if ((pid = fork()) < 0) {
        err_sys("fork error");
    } else if (pid > 0) {   /* parent */
        /* write lock entire file */
        if (write_lock(fd, 0, SEEK_SET, 0) < 0)
            err_sys("write_lock error");

        sleep(10);      /* wait for child to set lock and read data */

        if (waitpid(pid, NULL, 0) < 0)
            err_sys("waitpid error");

    } else {            /* child */
        sleep(5);      /* wait for parent to set lock */

        if (write_lock(fd, 0, SEEK_SET, 0) < 0) {
            if (errno == EACCES) {
                fprintf(stdout, "child write lock ERROR EACCES\n");
            } else if (errno == EAGAIN) {
                fprintf(stdout, "child write lock ERROR EAGAIN\n");
            } else {
                fprintf(stdout, "child write lock ERROR OTHER\n");
            }
            exit(-1);
        } else {
            fprintf(stdout, "child write lock RIGHT\n");
        }
    }
    exit(0);
}

