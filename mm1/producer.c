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
	char *shmChar = (char*) shmat(charID, NULL, SHM_RND);

		p(EMPTY, sem_id);
		p(MUTEX, sem_id);

		shmem[REAR.PID].PID=getpid();
		shmem[REAR.PID].letter=*shmChar;
		shmem[REAR.PID].size=atoi(argv[1]);
		shmem[REAR.PID].sec=atoi(argv[2]);
		shmem[REAR.PID].status=0;
		
		(*shmChar)++;

		//printf("Sent Job\n");	
		REAR.PID = (REAR.PID + 1) % bufferSize;		//updating FRONT
	

		v(MUTEX, sem_id);
		v(FULL, sem_id);

		
		printf("%d terminated with %d blocks in %d seconds\n", getpid(), atoi(argv[1]), atoi(argv[2]));



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

