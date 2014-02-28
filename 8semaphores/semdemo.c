/*
** semdemo.c -- demonstrates semaphore use as a file locking mechanism
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_RETRIES 10

union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

/*
** initsem() -- more-than-inspired by W. Richard Stevens' UNIX Network
** Programming 2nd edition, volume 2, lockvsem.c, page 295.
*/
int initsem(key_t key, int nsems)  /* key from ftok() */
{
	int i;
	union semun arg;
	struct semid_ds buf;
	struct sembuf sb;
	int semid;

    // 我们必须保证只有单个进程初始化文件上锁信号灯，因此给semget
    // 指定了IPC_CREAT | IPC_EXCL 标志
    // 如果创建成功，当前进程就调用semop将该信号灯的值初始化为1
    // 如果有多个进程几乎同时调用我们的my_lock函数，那么只有一个
    // 进程会创建出文件上锁信号灯（假设它尚未存在），接着初始化该
    // 信号灯的也是这个进程
	semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);

	if (semid >= 0) { /* we got it first */
		sb.sem_op = 1; // the value of sem_op is added to the semaphore's value
        sb.sem_flg = 0;
		arg.val = 1;

		printf("press return\n"); getchar();

		for(sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++) { 
			/* do a semop() to "free" the semaphores. */
			/* this sets the sem_otime field, as needed below. */
			if (semop(semid, &sb, 1) == -1) {
				int e = errno;
				semctl(semid, 0, IPC_RMID); /* clean up */
				errno = e;
				return -1; /* error, check errno */
			}
		}

	} else if (errno == EEXIST) { /* someone else got it first */
        
		int ready = 0;
        // 对于其他进程来说，第一个semget将返回一个EEXIST错误，
        // 它们于是再次调用semget，不过这次不指定IPC_CREAT和IPC_EXCL标志
		semid = semget(key, nsems, 0); /* get the id */
		if (semid < 0) return semid; /* error, check errno */

		/* wait for other process to initialize the semaphore: */
        // 遇到了11.2节中讲解System V信号灯的初始化时讨论过的同一竞争状态。
        // 为避免该竞争状态，发现文件上锁信号灯已存在的任何进程都必须以IPC_STAT
        // 命令调用semctl，以查看该信号灯的sem_otime值。
        // 如果该值不为0， 我们就知道创建该信号灯的进程已经对其初始化，并已调用semop
        // 如果该信号灯的sem_otime仍为0(这种情况非常罕见)，我们就sleep（1）再次尝试。
        // 我们限制了尝试次数，避免发生永久睡眠。
		arg.buf = &buf;
		for(i = 0; i < MAX_RETRIES && !ready; i++) {
			semctl(semid, nsems-1, IPC_STAT, arg);
			if (arg.buf->sem_otime != 0) {
				ready = 1;
			} else {
				sleep(1);
			}
		}
		if (!ready) {
			errno = ETIME;
			return -1;
		}
	} else {
		return semid; /* error, check errno */
	}

	return semid;
}

int main(void)
{
	key_t key;
	int semid;
	struct sembuf sb;
	
	sb.sem_num = 0;
	sb.sem_op = -1;  /* set to allocate resource */
	sb.sem_flg = SEM_UNDO;

	if ((key = ftok("semdemo.c", 'J')) == -1) {
		perror("ftok");
		exit(1);
	}

	/* grab the semaphore set created by seminit.c: */
	if ((semid = initsem(key, 1)) == -1) {
		perror("initsem");
		exit(1);
	}

	printf("Press return to lock: ");
	getchar();
	printf("Trying to lock...\n");

	if (semop(semid, &sb, 1) == -1) {
		perror("semop");
		exit(1);
	}

	printf("Locked.\n");
	printf("Press return to unlock: ");
	getchar();

	sb.sem_op = 1; /* free resource */
	if (semop(semid, &sb, 1) == -1) {
		perror("semop");
		exit(1);
	}

	printf("Unlocked\n");

	return 0;
}
