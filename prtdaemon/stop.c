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
	int     buffer;
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
	int structID, shmid, mutex_id, full_id, empty_id;

	if( (fp = fopen( "idFile", "w" )) == NULL ) {
                        printf( "Error Opening ID File\n" );
                        return 0;
                }
	fscanf(fp, "%d", structID);
	fscanf(fp, "%d", shmid);
	fscanf(fp, "%d", mutex_id);
	fscanf(fp, "%d", full_id);
	fscanf(fp, "%d", empty_id);
	fclose(fp);
	
	if ((semctl(mutex_id, 0, IPC_RMID, 0)) == -1)
       printf("ERROR in removing sem\n");
	if ((semctl(full_id, 0, IPC_RMID, 0)) == -1)
       printf("ERROR in removing sem\n");
	if ((semctl(empty_id, 0, IPC_RMID, 0)) == -1)
       printf("ERROR in removing sem\n");

	system("./rmshm");
	system("./rmFiles");
	system("rm idFile");
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

