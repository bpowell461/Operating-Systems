//Brad Powell
//3/22/21
//Print Daemon
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>


typedef struct jobReq {
        int PID;
        char letter;
        int size;
        int sec;
        int status;
}job;

void p(int s, int sem_id);
void v(int s, int sem_id);

#define MUTEX 0
#define EMPTY 2
#define FULL 1
#define FRONT shmem[bufferSize]
#define REAR  shmem[bufferSize+1]
int main(int argc, char *argv[])
{

	FILE *fp, *fopen();
        int rows, cols, bufferSize, shmid, sem_id, charID;

        if((fp = fopen("idFile","r")) == NULL)
        {
                printf("No file error");
                return(-1);
        }

	fscanf(fp, "%d", &shmid); //writing IDs to file
        fscanf(fp, "%d", &charID);
        fscanf(fp, "%d", &sem_id);
        fscanf(fp, "%d", &rows);
        fscanf(fp, "%d", &cols);
        fscanf(fp, "%d", &bufferSize);
        fclose(fp);


	job *shmem = (struct jobReq*) shmat(shmid, NULL, SHM_RND);

	
	if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");
	if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
		printf("ERROR in removing shmem.\n");
	if ((semctl(sem_id, 0, IPC_RMID, 0)) == -1)
     		 printf("ERROR in removing sem\n");

	system("./rmFiles");
	system("rm idFile");

	printf("Shutting down...\n");
	return 0;


}

void p(int s, int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = -1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("'P' error\n");
}

void v(int s, int sem_id)
{
struct sembuf sops;

sops.sem_num = s;
sops.sem_op = 1;
sops.sem_flg = 0;
if((semop(sem_id, &sops, 1)) == -1) printf("'V' error\n");
}

