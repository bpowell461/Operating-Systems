// Brad Powell
// CSC 460
// 2/3/2021
// Alpha2


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
if (N > 26)
{ 
	printf("Error: Out of Range!");
	N = 26;
}

printf("N: %d\n\n",N);

printf("ALPHA:PID\n\n");
char alpha = 'A';

int i;
for (i=0; i< N; i++)
{
    if(fork()==0)
    	alpha++;
}

printf("%c:%d\n",alpha,getpid());
sleep(1);

//printf("All Done.\n");

sleep(1);
return 0;
}

