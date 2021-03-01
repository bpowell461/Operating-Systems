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
        struct bbStruct *shmem;
        FILE *fopen(), *fp;

	if( (fp = fopen("/pub/os/bb/BBID.txt", "r")) == NULL ) 
	{
                printf( "Error - Cannot Open\n" );
                return 0;
        }
	
	fscanf( fp, "%i", &shmid );
	fclose( fp );

	shmem = ( struct bbStruct* ) shmat( shmid, NULL, SHM_RND );

	
	int i;
	for(i = 0; i<15 ; i++)
	{
	char Sid[10];
	snprintf(Sid,10, "%d", shmem->id);
	char c = ':';
	strncat(Sid, &c, 1);
	
	printf("%-10s%-20s\t%-10d\t%-30s\n",Sid,shmem->name,shmem->favNum,shmem->favFood);
	shmem++;
	}

	return 0;
}
