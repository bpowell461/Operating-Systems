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
	int structID, shmid, mutex_id, full_id, empty_id, buffersize;

	bufferStruct = (struct myVals*) shmat(structID, NULL, SHM_RND); //Array of Structs
	int *shmem = (int *) shmat(shmid, NULL, SHM_RND);
	

	if( (fp = fopen( "idFile", "w" )) == NULL ) {
                        printf( "Error Opening ID File\n" );
                        return 0;
                }
	fscanf(fp, "%d", structID);  //Scanning for ID's of Sem and Shm
	fscanf(fp, "%d", shmid);
	fscanf(fp, "%d", mutex_id);
	fscanf(fp, "%d", full_id);
	fscanf(fp, "%d", empty_id);
	fscanf(fp, "%d", buffersize);
	fclose(fp);
	int i;
	for(i=0; i<5; i++)	//Print and work 5 times
	{
		printf("User%d is working for 5 seconds.", getpid());
		sleep(5);			//Working
		p(EMPTY, empty_id);
		p(MUTEX, mutex_id);

		bufferStruct = bufferStruct + REAR; //Accessing Index of Struct in Array

		bufferStruct->pid=getpid(); //Setting the ID of the Struct
		sprintf(bufferStruct->name,"file%d", getpid()); //File name is File###


		char command[100];	//Bash Command
		sprintf(command, "curl -s http://api.quotable.io/random | cut -d':' -f4 | cut -d'\' -f2 > %s", bufferStruct->name);
		system(command);
	
		printf("User%d is printing file%d \n", getpid(),getpid());
		REAR = (REAR + 1) % buffersize;	//Submitting print request then updating REAR	
	

		v(MUTEX, mutex_id); //Print request submit
		v(FULL, full_id);
	}
	
	printf("User%d is logging off...", getpid());
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

