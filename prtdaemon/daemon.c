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


void p(int s, int sem_id);
void v(int s, int sem_id);

struct myVals
{
	int     pid;
	char    name[25];
};  /////// NOTE THE semicolon!


#define MUTEX 0
#define EMPTY 2
#define FULL 1
#define FRONT shmem[3]
#define REAR  shmem[4]
int main(int argc, char *argv[])
{
	FILE *fopen(), *fp;
	struct myVals *bufferStruct;
	int buffersize = atoi(argv[1]);
	int memsize = buffersize*sizeof(bufferStruct);
	int structID  =  shmget(IPC_PRIVATE, memsize, 0777);
	int shmid  =  shmget(IPC_PRIVATE, 4, 0777);
	if (shmid == -1)
    	{
    		printf("Could not get shared memory.\n");
    		return(0);
    	}

	bufferStruct = (struct myVals*) shmat(structID, NULL, SHM_RND);//initliazing sems and shm
	int *shmem = (int *) shmat(shmid, NULL, SHM_RND);
	
	FRONT = 0;
	REAR = 0;
	if( (fp = fopen( "idFile", "w" )) == NULL ) {
                        printf( "Error Opening ID File\n" );
                        return 0;
                }
	int mutex_id = semget (IPC_PRIVATE, 1, 0777);
	int full_id = semget (IPC_PRIVATE, 1, 0777);	//initializing sems
	int empty_id = semget (IPC_PRIVATE, 1, 0777);
	if ((mutex_id == -1) || (full_id == -1) || (empty_id == -1))
   	{
    		printf("SemGet Failed.\n");
    		return (0);
   	}

	semctl(mutex_id, MUTEX, SETVAL, 1);
	semctl(full_id, FULL, SETVAL, 0);
	semctl(empty_id, EMPTY, SETVAL, buffersize);

	fprintf(fp, "%d\n", structID); //writing IDs to file
	fprintf(fp, "%d\n", shmid);
	fprintf(fp, "%d\n", mutex_id);
	fprintf(fp, "%d\n", full_id);
	fprintf(fp, "%d\n", empty_id);
	fprintf(fp, "%d\n", buffersize);
	fclose(fp);
	
	while(1)
	{

		p(FULL, full_id);
		p(MUTEX, mutex_id);

		bufferStruct = bufferStruct + FRONT; //Access index of struct
		printf("Consumer gets User%d \n", bufferStruct->pid);
		FILE *textPtr;
		textPtr = fopen(bufferStruct->name, "r");
		char c;
		c = fgetc(textPtr);
		while (c != EOF)
   		 {
        		printf ("%c", c);	//printing out contents of file
        		c = fgetc(textPtr);
    		}	
		fclose(textPtr);
				
		FRONT = (FRONT + 1) % buffersize;		//updating FRONT
	

		v(MUTEX, mutex_id);
		v(EMPTY, empty_id);


	}

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

