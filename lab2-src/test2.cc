
#include <stdlib.h>
#include <stdio.h>

int allocations = 1000;

int
main( int argc, char **argv )
{
  printf("\n---- Running test2 ---\n");
  for ( int i = 1; i < allocations; i++ ) {
    char * p1 = (char *) malloc( i );
    *p1 = 100;
    char * p2 = (char *) malloc( 100 );
    free( p1 );
    free( p2 );
  }
  char * p3 = (char*) malloc( 100000 );
  *p3 = 0;
  printf(">>>> test2 passed\n\n");
  exit( 0 );
}

