/*
** Description: 
**     In order to show the id in parent and child process
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main()
{
    int i, pid;
    i = 12;
    int fd;

    if ((fd = open("test", O_CREAT | O_RDWR, S_IRWXU)) == -1) {
        printf("open failed \n");
        exit(1);
    } 

    if ((pid = fork()) == 0) { 
        //child
        printf("child pid:%d uid:%d ppid:%d euid:%d session id:%d gid:%d egid:%d\n",\
            getpid(), getuid(), getppid(), geteuid(), getsid(0), getgid(), getegid());
    } else { 
        // parent
        printf("parent pid:%d uid:%d ppid:%d euid:%d session id:%d gid:%d egid:%d\n",\
            getpid(), getuid(), getppid(), geteuid(), getsid(0), getgid(), getegid());
    }

    return 0;
}
