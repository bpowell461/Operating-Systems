#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

void THINK( );
void EAT( );
void take_forks( int i, char* state, int utenID, int* clockMem );
void put_forks( int i, char* state, int utenID );
void test( int i, char* state, int utenID );
void p( int s, int sem_id );
void v( int s, int sem_id );

// Define the possible diner states
#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT (myID+N-1)%N
#define RIGHT (myID+1)%N
int main(int argc, char *argv[])
{
	int i, semID, myID;

	semID=semget(IPC_PRIVATE, 5, 0777);
        if(semID==-1) {
                printf( "Error Getting Semaphores...\n" );
                return( -1 );
        }

	for(i=0; i<5; i++)
	{
		semctl(semID, i, SETVAL, 1); //All initialized to 1

	}

	myID=0;
	for(i=0; i<4; i++)
	{
		if(fork()==0)
		{
			myID++;
		}
		else
		{
			break;
		}
	}

	while(1)
	{
		for(i=0;i<myID;i++)
		{
			printf("\t");
		}
		printf("%d THINKING\n", myID);
		for(i=0; i<99999; i++); //busy loop

		for(i=0;i<myID;i++)
                {
                        printf("\t");
                }
		printf("%d HUNGRY\n", myID);
		for(i=0; i<99999; i++); //busy loop

		p(LEFT, semID);
		for(i=0; i<99999; i++);
		p(RIGHT, semID);

		for(i=0;i<myID;i++)
                {
                        printf("\t");
                }
		printf("%d EATING\n", myID);
                for(i=0; i<99999; i++); //busy loop

		v(LEFT, semID);
		for(i=0; i<99999; i++);
		v(RIGHT, semID);
		
	}


	return 0;
}
void p(int s, int sem_id) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s", "'P' error\n");
        }
}

// V Function for Semaphores
void v(int s, int sem_id) {
         struct sembuf sops;

         sops.sem_num = s;
         sops.sem_op = 1;
         sops.sem_flg = 0;
         if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s","'V' error\n");
         }
}
