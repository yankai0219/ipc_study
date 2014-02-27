/*
** Description: 
**     In order to show all the opened fd in parent process 
**     will be inherited by child process
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
        char *test = "child test cc cc cc cc cc cc cc cc \n";
        write(fd, test, strlen(test));
    } else { 
        // parent
        char *test = "parent test pp pp pp pp pp pp pp pp\n";
        write(fd, test, strlen(test));
    }

    return 0;
}
