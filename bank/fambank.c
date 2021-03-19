//Brad Powell
//CSC 460
//Fam Bank
//03/15/2021
//


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>

int main( int argc, char *argv[])
{

	FILE *fopen(), *fp;

	int N=16;

	int amount, SemID, memBalID, sems, process;

	int* balance;


	if(argc==1)
	{
		
		sems=1;
		if((fp = fopen ("bankdata", "w")) == NULL)
		{
			printf("Error Opening Bank Data: Line 35\n");
			return 0;
		}

		SemID=semget(IPC_PRIVATE,sems,0777);
		if(SemID==-1)
		{
			printf("Memory Request Error: Line 42\n");
			return 0;
		}
		semctl(SemID,0,SETVAL,1);

		memBalID=shmget(IPC_PRIVATE,4,0777);

		if(memBalID==-1)
		{
			printf("Memory Request Error: Line 51\n");
			return 0;
		}
		balance=(int*)shmat(memBalID,NULL,SHM_RND);
		*balance=1000;

		fprintf(fp,"%d\n",memBalID);
		fprintf(fp,"%d\n",SemID);
		fclose(fp);
	}
	else if (argc>=2)
	{
	
		int pid, id, i, j;

		pid = 0;
		id = 0;
		if(atoi(argv[1])>0 && atoi(argv[1])<=100)
		{
			if((fp = fopen ("bankdata", "r")) == NULL)
                        {
                                printf("Program not started: Line 68\n");
                                return 0;
                        }
			fscanf(fp,"%d",&memBalID);
                        fscanf(fp,"%d",&SemID);
                        fclose(fp);

			balance=(int*)shmat(memBalID,NULL,SHM_RND);

			for(i=0;i<N;i++)
			{
				amount = (int)pow(2,i);
				process=fork();
				if(process==0)
				{
					if(fork()==0)
					{
						amount=-1*amount;
					}
				break;
				}
				id++;
			}
			int intArg=atoi(argv[1]); 
			if(getpid()!=pid)
			{
				for(i=0;i<intArg;i++)
				{
					if(amount<0)
					{
						for(j=0;j<intArg;j++)
						{
							p(0,SemID);
							printf("%d + %d = %d\n", *balance, amount, (*balance + amount));
							*balance+=amount;
							v(0,SemID);
						}
					}
					else
					{
						for(j=0;j<intArg;j++)
						{
							p(0,SemID);
                                                        printf("%d + %d = %d\n", *balance, amount, (*balance+amount));
                                                        *balance+=amount;
                                                        v(0,SemID);
						}
					
					}
				}
			}	
		}
	
		else if(strcmp("balance",argv[1])==0)
		{
			if((fp = fopen ("bankdata", "r")) == NULL)
                        {
                                printf("Program not started: Line 68\n");
                                return 0;
                        }
                        fscanf(fp,"%d",&memBalID);
			fclose(fp);
			balance=(int*)shmat(memBalID,NULL,SHM_RND);
			printf("Balance: %d\n", *balance);
		}
		else if(strcmp("cleanup",argv[1])==0)
		{
			if((fp = fopen ("bankdata", "r")) == NULL)
               		{
                        	printf("Program not started: Line 80\n");
                        	return 0;
                	}
			fscanf(fp,"%d",&memBalID);
                	fscanf(fp,"%d",&SemID);
                	fclose(fp);

			balance=(int*)shmat(memBalID,NULL,SHM_RND);
			printf("Balance: %d\n", *balance);
			
			if((semctl(SemID,0,IPC_RMID,0))==-1)
			{
				printf("Error Removing Sem: Line 92\n");
			}
			if((shmctl(memBalID,IPC_RMID,NULL))==-1)
                        {
                                printf("Error Removing Shm: Line 96\n");
                        }

		}



	}
return 0;
}

p( int s, int sem_id ) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = -1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s", "'P' error\n");
        }
}

v( int s, int sem_id ) {
        struct sembuf sops;

        sops.sem_num = s;
        sops.sem_op = 1;
        sops.sem_flg = 0;
        if((semop(sem_id, &sops, 1)) == -1) {
                printf("%s","'V' error\n");
        }
}




