/*
** Description: 
**     Basic knowledge of fork
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

int main()
{
    int i, pid;
    int rv;
    i = 12;
    if ((pid = fork()) == 0) { 
        //child
        sleep(2);
        printf("child->i:%d\n", i);
        exit(2);
    } else { 
        // parent
        printf("parent->i:%d\n", i);
        i = 24;
        wait(&rv);
        printf("parent->child return value:%d\n", rv);
    }

    return 0;
}

