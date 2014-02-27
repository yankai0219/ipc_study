/*
** Description: 
**     1) This is an fork application, which is like ngx_spawn_process in NGINX.
**     2) This also shows us that: child process shared code segment with parent.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#define INVALID_PID -2

typedef struct proc_s proc_t;
typedef void(*spawn_proc_pt)(void *data);
void child_func1_handler(void *data);
void child_func2_handler(void *data);
int spawn_process(spawn_proc_pt proc, void *data, int flag);

struct proc_s {
    spawn_proc_pt  sub;
    void          *data;
    int            flag;
};

// child process information
proc_t proc_array[] = {
    { child_func1_handler,
      "1",
      11 },

    { child_func2_handler,
      "2",
      22 },

    { NULL,
      NULL,
      0 }
};

int main()
{
    proc_t *proc;

    for (proc = proc_array; proc->sub ; proc++) {
        spawn_process(proc->sub, proc->data , proc->flag);
    
    }
    return 0;
}


int spawn_process(spawn_proc_pt proc, void *data, int flag)
{
    // flag is used control something
    pid_t pid, child_pid;
    
    // socketpair can be added here. 

    pid = fork();
 
    switch (pid) {
    case -1:
        printf("fork() failed while spawning\n");
        return INVALID_PID;
    case 0:
        child_pid = getpid();
        proc(data);
        break;
    default:
        break;
    } 
    printf("parent-->childpid:%d\t flag:%d\n", pid, flag);

    return pid;
}

void child_func1_handler(void *data)
{
    char *test;

    test = (char*)data;
    printf("child_func1 data:%s\n", test);
    sleep(2);
    // for (;/* forever loop*/;) {
    // 
    // }
}
void child_func2_handler(void *data)
{
    char *test;

    test = (char*)data;
    printf("child_func2 data:%s\n", test);
    sleep(2);
    // for (;/* forever loop*/;) {
    // 
    // }
}
