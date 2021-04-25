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
int canRun(char ram[], int ramSize, int runSize);
int isFinished(job currJobs[], int numJobs);
void display(char ram[], int rows, int cols);
void displayJobs(job currJobs[], int numJobs);

#define MUTEX 0
#define EMPTY 2
#define FULL 1
#define END 3
#define FRONT shmem[bufferSize]
#define REAR  shmem[bufferSize+1]
int main(int argc, char *argv[])
{
	FILE *fopen(), *fp;
	int* numJobs;
	job* shmem;
	job* currJobs;
	char* shmChar;
	int i;
	int rows = atoi(argv[1]);
	int cols = atoi(argv[2]);
	int bufferSize = atoi(argv[3]);
	char* ram;
	int ramID = shmget(IPC_PRIVATE, sizeof(char)*rows*cols, 0777);
	int charID = shmget(IPC_PRIVATE, sizeof(char), 0777);
	int shmid = shmget(IPC_PRIVATE, (bufferSize)*sizeof(struct jobReq), 0777);
	int shmJobid = shmget(IPC_PRIVATE, sizeof(int), 0777);
	int curID = shmget(IPC_PRIVATE, bufferSize*sizeof(struct jobReq), 0777);
	int endID = shmget(IPC_PRIVATE, sizeof(int), 0777);
	if (shmid == -1)
        {
                printf("Could not get shared memory.\n");
                return(0);
        }
	shmem = (struct jobReq*) shmat(shmid, NULL, SHM_RND);
	shmChar = (char*) shmat(charID, NULL, SHM_RND);
	numJobs = (int* ) shmat(shmJobid, NULL, SHM_RND);
	int* endFlag = (int* ) shmat(endID, NULL, SHM_RND);
	currJobs = (struct jobReq*) shmat(curID, NULL, SHM_RND);
	ram = (char*) shmat(ramID, NULL, SHM_RND);
	*numJobs = 0;
	*shmChar = 'A';
	FRONT.PID = 0;
	REAR.PID = 0;
	*endFlag = 1;
	if( (fp = fopen( "idFile", "w" )) == NULL ) {
                        printf( "Error Opening ID File\n" );
                        return 0;
                }
	int sem_id = semget (IPC_PRIVATE, 3, 0777);
	if (sem_id == -1)
   	{
    		printf("SemGet Failed.\n");
    		return (0);
   	}

	semctl(sem_id, MUTEX, SETVAL, 1);
	semctl(sem_id, FULL, SETVAL, 0);
	semctl(sem_id, EMPTY, SETVAL, bufferSize);

	fprintf(fp, "%d\n", shmid); //writing IDs to file
	fprintf(fp, "%d\n", charID);
	fprintf(fp, "%d\n", sem_id);
	fprintf(fp, "%d\n", rows);
	fprintf(fp, "%d\n", cols);
	fprintf(fp, "%d\n", bufferSize);
	fprintf(fp, "%d\n", endID);
	fclose(fp);
	
	int myID;

	myID=0;

	for(i=0; i<2; i++)
	{
		if(fork()!=0) break;
		myID++;
	}


	for(i=0; i<rows*cols;i++)
	{
		ram[i]='.';

	}

	//display(ram, rows, cols);
	if(myID==0)
	{
		while(*endFlag!=0)
		{
			p(FULL, sem_id);
			p(MUTEX, sem_id);

			currJobs[(*numJobs)]=shmem[FRONT.PID];
			(*numJobs)++;	
			FRONT.PID = (FRONT.PID + 1) % bufferSize;//updating FRONT
			//printf("Received Job\n");	

			v(MUTEX, sem_id);
			v(EMPTY, sem_id);
			

		}
                

       }
	else if(myID==1)
	{
		int j = 0;

		int k;
		int ramIndex;
		while(*endFlag!=0)
		{
			for(j=0; j<*numJobs;j++)
			{
				int ramSize=rows*cols;
				ramIndex = canRun(ram, ramSize, currJobs[j].size);
			//	printf("%c can fit at index %d\n", currJobs[j].letter, ramIndex);
				if(ramIndex>=0 && currJobs[j].status != -1)
				{
				
					currJobs[j].status=1;
					for(k=ramIndex; k<currJobs[j].size; k++)
					{
						ram[k]=currJobs[j].letter;
					}
				
				}
				if(currJobs[j].sec==0)
				{
					currJobs[j].status=-1;
					for(k=0; k<rows*cols; k++)
					{
						if(ram[k]==currJobs[j].letter)
						{
							ram[k]='.';
						}

					}
					
				}
			}
		if(*numJobs>=bufferSize)
                        {
                        if(isFinished(currJobs, *numJobs))
                         break;
                        }

		sleep(1);
		}


	}
	else if(myID==2)
	{

		int r,c,j;
		int ramIndex=0;

		while(*endFlag!=0)
		{
		  displayJobs(currJobs, *numJobs);
		  display(ram, rows, cols);
		  for(j=0; j<*numJobs;j++)
                  {
 			if(currJobs[j].status==1)
                        {
                                        currJobs[j].sec--;
                         }	
		  }
		  sleep(1);
		

		}

	}
	//system("killall consumer");
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


int canRun(char ram[], int ramSize, int runSize)
{
	int i, index, flag;
	index = 0;
	flag=0;
	for(i = 0; i<ramSize; i++)
	{
	 	if(ram[i]=='.')
		{
			index++;
		}
		else
		{
			index=0;
		}
	}
	if(index>=runSize)
        {
          return i-index;
        }
         else
         {
           return -1;
         }

}
int isFinished(job currJobs[], int numJobs)
{
	int i;
	for(i=0; i<numJobs; i++)
	{
		if(currJobs[i].status >= 0)
			return 0;	

	}
	return 1;
}
void displayJobs(job currJobs[], int numJobs)
{

	int i;
	printf("ID  PID    SIZE   SEC\n");

	for(i=0; i<numJobs; i++)
        {
                printf("%c  %d    %d   %d\n", currJobs[i].letter, currJobs[i].PID, currJobs[i].size, currJobs[i].sec);
        }

	printf("\n");
}
void display(char ram[], int rows, int cols)
{
		int r,c;
                int ramIndex=0;

                for(r=0; r<rows; r++)
                {
                        for(c=0; c<cols; c++)
                        {
                                printf("%c",ram[ramIndex]);
                                ramIndex++;
                        }
                        printf("\n");
                }

		printf("\n");

}
