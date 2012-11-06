#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int
main( int argc, char **argv )
{
	unsigned long gb1 = 1024*1024*1024;
	unsigned long gb2 = 0x7FFFFFE0;
	unsigned long gb4 = 0xFFFFFFFF;
	char* ret;
	ret = (char*)malloc(gb4);
	assert (ret == (char*)0);
	ret = (char*)malloc(gb1);
	assert (ret != (char*)0);
	free(ret);
	pid_t pid = getpid();
	char command[1024];
	sprintf(command, "pmap %d\n", pid);
	system(command);

	exit(0);
}

