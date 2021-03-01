// Brad Powell
// CSC 460
// 2/26/2021
// Shared Bulletin Board


#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>

struct bbStruct {
        int id;
        char name[20];
        int favNum;
        char favFood[30];
};

int main( int argc, char** argv ) {

        int shmid;
	char* newFood;
        struct bbStruct *shmem;
        FILE *fopen(), *fp;

	if( (fp = fopen("/pub/os/bb/BBID.txt", "r")) == NULL ) 
	{
                printf( "Error - Cannot Open\n" );
                return 0;
        }
	
	if (argc > 2)
	{
		printf("Invalid Input\n");
		return 0;

	}

	newFood = argv[1];
	
        while(strlen(newFood) > 30)
	{
		newFood[strlen(newFood)-1]='\0';
	}

	fscanf(fp, "%i", &shmid);
	fclose(fp);

	shmem = (struct bbStruct*) shmat(shmid, NULL, SHM_RND);

	shmem = shmem + 10; //offset of Brad

	strcpy(shmem->favFood,newFood);

	return 0;
}
