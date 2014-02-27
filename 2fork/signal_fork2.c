/*
** Description: 
**     In order to show the signal in parent and child process
**     [note]: sigsuspend can be placed in parent and children process
**             every process wait signal by itsself;
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
#include <signal.h>

typedef struct signal_s signal_t;
struct signal_s {
    int   signo;
    char *signame;
    void (*handler)(int signo);
};

int  init_signal();
void alam_handler(int signo);
void chld_handler(int signo);
void int_handler(int signo);
void quit_handler(int signo);


int      yk_alam, yk_chld, yk_int, yk_quit;

signal_t signals[] = {
    { SIGALRM, "SIGALRM", alam_handler },
    { SIGCHLD, "SIGCHLD", chld_handler },
    { SIGINT, "SIGINT", int_handler },
    { SIGQUIT, "SIGQUIT", quit_handler },
    { 0, NULL, NULL }
};


int main()
{
    int      pid;
    sigset_t set;
     
    init_signal();

    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        printf("sigprocmask() error\n");
        return -1;
    }

    sigemptyset(&set);
    

    if ((pid = fork()) == 0) { 
        //child
        fprintf(stdout, "child wait signal is trigger\n");
        sigsuspend(&set);
        
        if (yk_quit == 1) {
            printf("child quit signal is trigger\n");
        } else if (yk_alam == 1) {
            printf("child alam signal is trigger\n");
        } else if (yk_int == 1) {
            printf("child int signal is trigger\n");
        } else if (yk_chld == 1) {
            printf("child chld signal is trigger\n");
        }
    } else { 
        // parent
        fprintf(stdout, "parent wait signal is trigger\n");
        sigsuspend(&set);
        
        if (yk_quit == 1) {
            printf("parent quit signal is trigger\n");
        } else if (yk_alam == 1) {
            printf("parent alam signal is trigger\n");
        } else if (yk_int == 1) {
            printf("parent int signal is trigger\n");
        } else if (yk_chld == 1) {
            printf("parent chld signal is trigger\n");
        }
    }

    return 0;
}

int init_signal()
{
    signal_t        *sig;
    struct sigaction sa;

    for (sig = signals; sig->signo != 0; sig++) {
        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_handler = sig->handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(sig->signo, &sa, NULL) == -1) {
            fprintf(stderr, "add %dth signal action:%d error\n", (int)(sig - signals), (int)sig->signo);
            return -1;
        }
    }
    return 0;
}

void quit_handler(int signo) 
{
    if (signo == SIGQUIT) {
        yk_quit = 1;
        printf("this is a signal called quit\n");
    } else {
        printf("this should be quit, However it is NOT\n");
    }
}

void int_handler(int signo) 
{
    if (signo == SIGINT) {
        yk_int = 1;
        printf("this is a signal called int\n");
    } else {
        printf("this should be int, However it is NOT\n");
    }
}

void chld_handler(int signo) 
{
    if (signo == SIGCHLD) {
        yk_chld = 1;
        printf("this is a signal called chld\n");
    } else {
        printf("this should be chld, However it is NOT\n");
    }
}

void alam_handler(int signo) 
{
    if (signo == SIGALRM) {
        yk_alam = 1;
        printf("this is a signal called alam\n");
    } else {
        printf("this should be alam, However it is NOT\n");
    }
}
