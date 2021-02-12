// Brad Powell
// CSC 460
// 2/5/2021
// Slow Sync
#include <stdio.h>

main(int argc, char *argv[])
{

FILE *fopen(), *fp;

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

/*****  Open File to write a value  *****/

if((fp = fopen("syncfile","w")) == NULL)
    {
    printf(":( could not open myLittleFile to write.\n");
    return(0);
    }

char alph[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char letters[N];

int i;
for (i=0; i<N; i++)
{
letters[i] = alph[i];
}

/*****  Write into syncfile ID of whose turn.  *****/

fprintf(fp,"%d",0);
fclose(fp);


/*****  Create Child process and give unique myID *****/
int  myID = 0;
int  otherID = 1;
for (i=0; i< N; i++)
{
if (fork() == 0)
{
   myID=(myID+1)%N;
   otherID=(otherID+1)%N;
}
}

/*****  LOOP 100 Times  *****/

i = 0;
int  found;

for (i=0; i<N; i++)
{

     /***** Repeatedly check file until myID is found *****/

     found = -1;
     while (found != myID)
     {
	//the loop is getting stuck here for some reason
         if((fp = fopen("syncfile","r")) == NULL)
             {
             printf(":( could not open syncfile to read.\n");
             return(0);
             }

             fscanf(fp,"%d",&found);
             fclose(fp);
     } 
     // It must be my turn to do something.....

     printf("%c:%d\n",letters[myID],getpid());

     // Update file to allow otherID to go 

     if((fp = fopen("syncfile","w")) == NULL)
         {
         printf(":( could not open myLittleFile to write.\n");
         return(0);
         }
     fprintf(fp,"%d",otherID);
     fclose(fp);

}

    return(0);

}
