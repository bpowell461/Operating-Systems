#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

void think();
void eat();
void take_forks(int myID, int* state, int semID);
void put_forks(int myID, int* state, int semID);
void test(int myID, int* state, int semID);
void printStates(int* clock, int* state);
void p( int s, int sem_id );
void v( int s, int sem_id );
int checkProcesses(int* state);
#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define DEAD 3
#define LEFT (myID+N-1)%N
#define RIGHT (myID+1)%N

int main(int argc, char *argv[])
{
	int i, semID, myID, clockID, stateID;
	int* state;
	int* clock;
	

	clockID = shmget(IPC_PRIVATE, 4, 0777 );
        if( clockID == -1 ) {
                printf( "Error Getting Clock Shared Memory...\n" );
                return( -1 );
        }
        clock = shmat(clockID, NULL, SHM_RND);

	*clock = 0; //0 seconds and will increment

	stateID=shmget(IPC_PRIVATE, N*sizeof(int), 0777 );
        if( stateID == -1 ) {
                printf( "Error Getting Clock Shared Memory...\n" );
                return( -1 );
        }
	state = shmat(stateID, NULL, SHM_RND);
	
	for(i=0; i<5; i++)
	{
		state[i]=THINKING;	
	}

	semID=semget(IPC_PRIVATE, 6, 0777);
        if(semID==-1) {
                printf( "Error Getting Semaphores...\n" );
                return( -1 );
        }

	for(i=0; i<5; i++)
	{
		semctl(semID, i, SETVAL, 0); //All initialized to 0

	}
	semctl(semID, 5, SETVAL, 1);

	myID=0;
	for(i=0; i<5; i++) //Forking 6 Processes (5 Phils, 1 Monitor)
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

	if(myID < 5) //Make sure process is not the main process
	{
		while(*clock < 60)
		{
			think();
	
			take_forks(myID, state, semID);

			eat();

			put_forks(myID, state, semID);
		
		}

		state[myID]=DEAD;
	}
	else
	{
		while(1)
		{
			printStates(clock, state);
			sleep(1);

			(*clock)++;

			if(checkProcesses(state)==1)
		        {
				break;
			}

			if(*clock >= 60) //If the clock reaches 60 then finish everything
			{
					state[myID]=DEAD;
					break;

			}


		}

	}

	if(checkProcesses(state)==1)
	{
		printStates(clock, state);
		if((shmctl( stateID, IPC_RMID, NULL ) ) == - 1 )
                        printf( "Error Removing Shared Memory\n" );

                if((shmctl( clockID, IPC_RMID, NULL ) ) == -1 )
                        printf( "Error Removing Shared Memory\n" );
                if((semctl( semID, 0, IPC_RMID, 0 ) ) == - 1 )
                        printf( "Error Removing Semaphores\n" );
	}	

	return 0;
}
void eat()
{
	sleep((rand()%3)+1);	
}
void think()
{
	sleep((rand()%5)+1);
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

int checkProcesses(int* state)
{
	int i;
	for(i=0;i<N;i++)
	{
		if(state[i]!=DEAD)
			return 0;

	}
	return 1;
}

void take_forks(int myID, int* state, int semID)
{
	p(5,semID);
	state[myID]=HUNGRY;
	test(myID, state, semID);
	v(5, semID);
	p(myID, semID);
}
void put_forks(int myID, int* state, int semID)
{
	p(5,semID);
        state[myID]=HUNGRY;
	test(LEFT,state, semID);
	test(RIGHT,state,semID);
	v(5, semID);

}
void test(int myID, int* state, int semID)
{
	if(state[myID]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING)
	{
		state[myID]=EATING;
		v(myID, semID);
	}

}
void printStates(int* clock, int* state)
{
	int i;

	printf("%d:\t", *clock);
	for(i=0;i<N;i++)
	{
		switch(state[i])
		{
			case THINKING:
			printf("thinking ");
			break;

			case HUNGRY:
                        printf("hungry ");
                        break;

			case EATING:
                        printf("eating ");
                        break;

			case DEAD:
                        printf("dead ");
                        break;
		}

	}
	printf("\n");	
}
