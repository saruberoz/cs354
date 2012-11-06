
#include <elf.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>

typedef void (*FUNCPTR)();

// Table with the functions that the executables will need.
// Insert only the functions needed to make the example programs run.
FUNCPTR FUNC_TABLE[] = 
{
	(FUNCPTR) printf,
	(FUNCPTR) scanf,
	(FUNCPTR) gets,
	(FUNCPTR) puts,
	(FUNCPTR) putchar,
	(FUNCPTR) strlen,
	(FUNCPTR) strcpy,
};

//
// Load a program into memory at the position indicated
// in the executable file and start running it.
//
int load_and_run(char * progname, int argc, char ** argv ) 
{
  Elf32_Ehdr ehdr;
  Elf32_Phdr phdr;
  Elf32_Addr addr;
  int i;
  FILE *fp;
  
  // Open progname
  fp = fopen(argv[1], "r");
  if (fp == 0) {
    fprintf(stderr, "%s: can't open %s\n",argv[0], argv[1]);
    exit(1);
  }
  
  // Read ehdr
  if ( fread( &ehdr, sizeof(ehdr), 1, fp ) != 1 ) {
    fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
    exit(1);
  }
  
  // Get entry point address from ehdr.e_entry and store it in a
  // function pointer "mystart".
  FUNCPTR mystart = (FUNCPTR) ehdr.e_entry;
  
  // Iterate over all phdrs
  // Program headers. Load program
  //for ( i = 0; i < ehdr.e_phnum; i++ ) {
  for(i = 0 ; i < ehdr.e_phnum ; i++) {
    // read phdr
    fseek( fp, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET );
    if ( fread( &phdr, sizeof(phdr), 1, fp ) != 1 ) {
      fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
      exit(1);
    }
    // allocate memory mapping at phdr.p_vaddr of size phdr.p_memsz using
    // addr = mmap((void*) startAddr, mappedSize, PROT_READ|PROT_WRITE| PROT_EXEC, 
    //              MAP_PRIVATE|MAP_FIXED|MAP_ANON, -1, 0);
    addr = (Elf32_Addr) mmap ((void*) phdr.p_vaddr, phdr.p_memsz, PROT_READ|PROT_WRITE| PROT_EXEC, 
                  MAP_PRIVATE|MAP_FIXED|MAP_ANON, -1, 0);
    // Make sure addr returned is the one requested
    if(addr != phdr.p_vaddr) {
      fprintf(stderr, "Address value not same");
      exit(1);
    }
    
    
    // Read the program from phdr.p_offset, phdr.p_filesz bytes and store it
    // into phdr.p_vaddr. Do this only if phdr.p_filesz>0.
    if(phdr.p_filesz > 0){
      fseek( fp, phdr.p_offset, SEEK_SET );
      if ( fread( (void*)phdr.p_vaddr, phdr.p_filesz, 1, fp ) != 1 ) {
	fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	exit(1);
      }
    }
  //}
  }

  // Call the "mystart" function using the pointer obtained above.
  // Pass as arguments FUNC_TABLE, argc, argv
  (mystart) (FUNC_TABLE, argc, argv);
}

main(argc, argv)
     int argc;
     char **argv;
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s filename\n", argv[0]);
    exit(1);
  }

  load_and_run(argv[1], argc, argv);

  exit(0);
}
