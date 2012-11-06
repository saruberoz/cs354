#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int
main( int argc, char **argv )
{
	unsigned long sixteenk = 16*1024;
	char* one = (char*) malloc(sixteenk - 32);	
	free(one);

	char *two = (char *) malloc(sixteenk - 64);
	free(two);

	char* three = (char*)sbrk(sixteenk);

	char* four = (char*)malloc(sixteenk * 2 - 64);



	pid_t pid = getpid();
	char command[1024];
	sprintf(command, "pmap %d\n", pid);
	system(command);

	printf("\nthree is %x\n", three);
	printf("four is %x\n", four);

	assert (four > three + sixteenk);

	return 0;
}

