#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    int   msqid;
    key_t key;
    key = ftok("test", 'b');
    msqid = msgget(key, 0666 | IPC_CREAT);
    printf("msqid:%d\n", msqid);

    return 0;
}
