/*
** Description:
**     basic use of signal
**     more usage is in 2fork directory.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
int test_quit;
static void sig_quit(int signo)
{
    printf("caught SIGQUIT\n");
    test_quit = 1;
}
int main()
{
    sigset_t oldmask;
    sigset_t pendmask;
    sigset_t waitmask;
    struct sigaction act,oact;
    memset(&act,0,sizeof(struct sigaction));
    
    act.sa_handler = sig_quit;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGQUIT,&act,&oact);
    
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    
    sigprocmask(SIG_BLOCK, &act.sa_mask, &oldmask);
    printf("Please entrl CTRL+\\ before sleep\n");
    sleep (5);
    printf("after sleep\n");
    
    sigpending(&pendmask);
    if (sigismember(&pendmask, SIGQUIT)) {
        printf("\nSIGQUIT pending\n");
    }
    sigemptyset(&waitmask);
    sigsuspend(&waitmask);
    printf("hello,world\n");

    if(test_quit == 1)
        printf("test_quit == 1\n");
    else
       printf("test_quit is not 1\n");
    printf("before sleep\n");
    sleep(5);
    printf("after sleep\n");
    return (0);
}
