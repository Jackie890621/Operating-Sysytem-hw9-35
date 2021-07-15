#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct{
	long cat[10];
	int arg;
} share_data;

int main(int argc, char *argv[])
{
	//判斷使用者是否只輸入一個數字
	if (argc == 1) {
                printf("You need to input a number\n");
		return 1;
        } else if (argc > 2) {
                printf("You only need to input one number\n");
		return 1;
        }

	share_data *data;
	int n = atoi(argv[1]);
	int status;
	int id;
	int key = 1;
	long fact_n = 1;
	long fact_n1 = 1;
	long fact_2n = 1;
	int temp = 1;
	pid_t parent_pid, child_pid;

	if (n <= 0) { //確認使用者輸入數字大於零
		printf("You need to input a number > 0\n");
	} else {
		parent_pid = fork();
		if (parent_pid == 0) { //child process
			//create share memory id
			id = shmget(key, sizeof(share_data), 0666|IPC_CREAT);
			//check id
                        if (id == -1) {
                                printf("id error\n");
                                return 1;
                        }
			// use the id to create share memory
                        data = shmat(id, NULL, 0);
			//check share memory
                        if (data == (void *) -1) {
                                printf("shm address error\n");
                                return 1;
                        }
			//initiallize
                        data->arg = n;
			//產生費式數列
			
			for (int i = 1; i <= data->arg; i++) {
				fact_n *= i;
				fact_n1 *= (i + 1);
				for (int j = temp + 1; j <= (2 * i); j++) {
					fact_2n *= j;
					temp = j;
				}
				data->cat[i] = fact_2n / (fact_n * fact_n1);	
			}

			exit(0);
		} else if (parent_pid > 0) { //parent process
			//create share memory id
			id = shmget(key, sizeof(share_data), 0666|IPC_CREAT);
			//check id
			if (id == -1) {
				printf("id error\n");
				return 1;
			} else {
			//	printf("Create a shared-memory segment, segment_id = %d\n", id);
			}
			// use the id to create share memory
                        data = shmat(id, NULL, 0);
                        //check share memory
                        if (data == (void *) -1) {
                                printf("shm address error\n");
                                return 1;
                        }
                        //initiallize
                        data->arg = n;
			
			child_pid = wait(&status);
			for (int i = 1; i <= data->arg; i++) {
				printf("%ld ", data->cat[i]);
			}
			printf("\n");
			//detach the share memory
			if (shmdt(data) == -1) {
				printf("detach error\n");
			}
		} else {
			printf("fork error\n");
		}
	}

	printf("Discussion:\n");
	printf("1. \n");
	printf("Share memory is faster than process copy because process copy\n");
	printf("need to wait a process to finish and then copy\n");
	printf("2. \n");
	printf("Share memory only need to create a share-segment. It's much more efficient\n");
	printf("and page fault occurs less frequently\n");
	return 0;
}
