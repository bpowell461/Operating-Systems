// Brad Powell
// CSC 460
// 2/1/2021
// Pyramid Scheme


#include <stdio.h>

int main(int argc, char * argv[])
{
int N;

if (argc == 1)
{
  N = 0;
}
else
{
  N = atoi(argv[1]);
}

if (N < 1)
{ 
	N = 1;
}
if (N > 5)
{ 
	printf("Error: Out of Range!");
	N = 0;
}

printf("N: %d\n\n",N);

printf("GEN\tPID\tPPID\n\n");

if(N!=0)
{
int i;
for (i=0; i< N; i++)
{
    fork();
    printf("%d\t%d\t%d\n",i,getpid(),getppid());
}
}
sleep(1);

printf("All Done.\n");

sleep(1);
return 0;
}

