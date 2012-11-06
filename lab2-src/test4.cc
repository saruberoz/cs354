#include <stdlib.h>
#include <stdio.h>

#define space ((16*1024) - 16)

int
main( int argc, char **argv )
{
  /*char* array[space / 24]; 
  int permutation [space / 24]; // random permutation 
  int i, j, k;
  int test = 10;
  for (i = 1; i <= test; i++) {
	for (j = 0; j < space / (i*8 + 16); j++) {
		// malloc all 16 KB in i*8 B chunks
		array[j] = (char*)malloc(i*8);
		permutation[j] = j;
	}


	for (k = 0; k < j; k++) {
		int a = rand() % j;
		int b = rand() % j;
		if (a == b) continue;
		permutation[a] = permutation[a] ^ permutation[b];
		permutation[b] = permutation[a] ^ permutation[b];
		permutation[a] = permutation[a] ^ permutation[b];
	}
	for (k = 0; k < j; k++) {
		assert(permutation[k] < j);
		assert(array[permutation[k]] != 0);
		free((char*)array[permutation[k]]);
		array[permutation[k]] = 0;
	}
  }*/
	
  printf("\n---- Running test4 ---\n");

  int * p = (int *) malloc( 24 );
  *p = 24;
  int *a = (int*) malloc (40);
  *a = 40;

  int *b = (int *) malloc (32);
  *b = 32;
  
  free(b);

  int *c = (int *) malloc(48);
 
  free(p);
  p = (int *) malloc (32);
  free(p);

  int *d = (int *) malloc (8);
  free (d);

  int *e;
  free(e);
  free(a);
  free(b);
  free(c);

  a = (int *) malloc (512);
  free(a);

  free(a);

  printf(">>>> test4 passed\n\n");

  return 0;
}
