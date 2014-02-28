#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdio.h>

int main()
{
    key_t key;
    int semid;
    
    key = ftok("test", 'E');
    semid = semget(key, 10, 0666 | IPC_CREAT);
    printf("semid:%d\n", semid);
    
    return 0;
}
