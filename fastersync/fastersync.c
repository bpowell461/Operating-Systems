#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#define TURN shmem[0]

main(int argc, char *argv[])
{



int i, shmid, *shmem;
/*****  Make a note of "who" is the first Process *****/

int firstID = getpid();


/*****  Get Shared Memory and ID *****/

shmid  =  shmget(IPC_PRIVATE, sizeof(int), 0770);
if (shmid == -1)
    {
    printf("Could not get shared memory.\n");
    return(0);
    }


/****   Attach to the shared memory  ******/

shmem = (int *) shmat(shmid, NULL, SHM_RND);


/****  Initialize the shared memory ****/

TURN = 0;

int N;

if (argc == 1)
{
  N = 1;
}
else
{
  N = atoi(argv[1]);
}

if (N < 1)
{ 
	N = 1;
}
if (N > 26)
{ 
	printf("Error: Out of Range!");
	N = 26;
}
char alph[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char letters[N];

for (i=0; i<N; i++)
{
letters[i] = alph[i];
}

int  myID = 0;
int  otherID = 1;
for (i=0; i< N-1; i++)
{
if (fork() == 0)
{
   myID=(myID+1)%N;
   otherID=(otherID+1)%N;
}
else
{
break;
}
}


 
/*************  BODY  OF  PROGRAM     ***********/
if (fork())
{       
	for(i=0; i<N; i++) 
	{
        // PARENT //
        while(TURN == myID);  /** busy wait for Child **/
	//printf("PARENT\n");
	printf("%c:%d\n",letters[myID],getpid()); //Print on Parent
	TURN = (TURN + 1)%N;
	}
}
else
{               //  CHILD  //
	for(i=0; i<N; i++)
        {
        while(TURN == otherID);  /** busy wait for Parent **/	
        //printf("CHILD\n");
        TURN = (TURN + 2)%N; //Increment Turn to next ID
	}
}
sleep(1);

/****   Detach and clean-up the shared memory  ******/

if (shmdt(shmem) == -1 ) printf("shmgm: ERROR in detaching.\n");

if (firstID == getpid())         // ONLY need one process to do this
if ((shmctl(shmid, IPC_RMID, NULL)) == -1)
    printf("ERROR removing shmem.\n");

//printf("alldone\n");
}

