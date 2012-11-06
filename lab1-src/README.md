CS354 Lab1

ELF Reader and Loader

Class Notes

Frequently Asked Questions

Goal

The goal of this lab is to write an ELF executable file reader and write a simple loader to load the program in memory and run it.
Part 1. Writing elfinfo

The UNIX OS allows the execution of programs by user request. A UNIX program is a file in a special format that is produced by tools such as compilers and assemblers. In this lab you are going to get familiarized with the format of executable files.

There are several formats of executable files. The most common ones are a.out and elf. a.out is the oldest one and it is used mainly in BSD \unix. Elf is used in Solaris and other System V flavors of \unix. You can find a description of this format in chapter 5 of ``SunOS Linker and Libraries Manual'' The C description of elf is in the file /usr/include/sys/elf.h.

1. First download the file lab1-src.tar.gz and copy it into the directory lab1-src. Then untar the file
gunzip lab1-src.tar.gz
tar -xvf lab1-src.tar
You are going to write a program that will display the header information and the symbol table contained in an elf file.

You may need to add the path to the gcc version 3 compiler at the beginning of your path. 
export PATH=/opt/csw/gcc3/bin:$PATH
Also, two versions of elfinfo are given: elfinfo.c and elfinfo-mmap.c Try to use the mmap implementation to get experience with mmap. 
2.Printing Execution Information And Symbol Table. Every elf file has an elf-header Elf32_Ehdr, zero or more program-headers Elf32_Phdr, and zero or more section-headers Elf32_Shdr. The structure of each header is described in /usr/include/sys/elf.h. The headers contain information such as the machine type, the starting point, number of sections, size of the sections, symbol tables, etc. This information is used by the OS when a program is loaded for execution. You will write a program that given the name of an elf file it displays the contents of the elf-header, the program-headers, the section-headers, and the symbol tables. Print as much in formation as possible of the headers using nice formatting (nice formatting will also be graded).

Before you start writing your program I suggest you to read the material about elf that was described above. Also there is a simple program elfinfo/elfinfo.c that parses an elf file and prints some basic values.

See the header file /usr/include/sys/elf.h for the names of the constants as well as chapter 7 "Object File Format" in Linker and Libraries Guide.

The output of your program should be the same as the one of "elfdump" such as:
elfdump -ecps elfinfo > out.elfdump
elfinfo elfinfo > out.elfinfo
diff -b -w -c out.elfdump elfinfo.out
There should not be differences in the output

Also try 
elfdump -ecps /usr/lib/libc.so > out.elfdump
elfinfo /usr/lib/libc.so > out.elfinfo
diff -b -w -c out.elfdump elfinfo
There should not be differences in the output for the program headers, section headers and symbol tables. You do not need to print the other extra information.

You will be able to use your new command ``elfinfo'' on any executable program or shared library:
        > elfinfo elfinfo
	> elfinfo /usr/bin/ls
	> elfinfo /usr/lib/libc.so

Note: Do not use libelf.
Part 2. Building a loader

In this section you will build a loader program that will be able to load an executable file into memory and then execute it.

1. The Makefile in this directory contains rules to build the loader and the example tests. In the Makefile you will see that the examlpe executable files loaded by the loader are built in a special way. 
hello: hello.c mylib.o mapfile
    gcc -g -c hello.c
    ld -dn -e mystart -M mapfile hello.o mylib.o -o hello
The "-dn" flag passed to the linker tells that the file is built statically without shared libraries. The "-e mystart" flag tells that the starting point of the executable is the function "mystart" that will call main(). The "-M mapfile" flag tells that the linker should be using the "mapfile" information. "mapfile" contains:
text    = LOAD ?RX V0x40000000;
text    : $PROGBITS ?A!W;
This tells that the program should start at address 0x40000000. We choose this address since it is above the loader program, beyond the heap and before the shared libraries. "hello.o " is the hello object file that contains main(). "mylib.o" is an object file that contains "mystart" and the  functions that the executables need. 

Type "make" and ignore for now the errors. Then type "elfinfo hello". You will see that the starting address of the program is 0x40000000

2. The file loader.c  contains the starting code for the loader. 
The loader is a program that will take the name of the executable file to execute, it will load the executable in memory at the address indicated in the executable and then call the function in the entry point. When your loader is complete you will be able to run the following command:
> loader hello

-------------------------------------
Welcome to CS354 Operating Systems

. To implement the loader add the code missing in "load_and_run(char * progname, int argc, char ** argv ) ". Follow the comments:
int load_and_run(char * progname, int argc, char ** argv ) 
{
  // Open progname

  // Read ehdr

  // Get entry point address from ehdr.e_entry and store it in function pointer "mystart".

  // Iterate over all phdr program headers.
  for ( i = 0; i < ehdr.e_phnum; i++ ) {
    // read phdr

    // allocate memory mapping at phdr.p_vaddr of size phdr.p_memsz. Make sure that the address is aligned to a page (8192) and also the memory required is a multiple of a page size. To mmap th eprogram sections use
    // addr = mmap((void*) startAddr, mappedSize, PROT_READ|PROT_WRITE| PROT_EXEC,
    //              MAP_PRIVATE|MAP_FIXED|MAP_ANON, -1, 0);
    // Make sure addr returned is the one requested

    // Read the program from phdr.p_offset, phdr.p_filesz bytes and store it 
    // into phdr.p_vaddr. Do this only if phdr.p_filesz>0.
  }

  // Call the "mystart" function using the pointer obtained above and
  // pass as arguments FUNC_TABLE, argc, argv
}
See the man pages of mmap if you have questions about its usage.

After you complete this part you should be able to run "loader hello".

4. The FUNC_TABLE is an array of pointers to the functions that are needed by the test programs that the loader loads. So far this table only has the entry "printf". This table is initialized in loader.c and then a pointer to the table is passed to the "mystart" entry point. The "mystart" entry point is defined in "mylib.c" where also it defines a "printf" wrapper that calls the real printf by calling the entry 0 of FUNC_TABLE. You will need to add more entries to the table and function wrappers to make sure that the other examples run: 
loader hello
loader invert
loader matrix
loader qsort
loader fact 

You will be done with this part when you are able to execute these files.
Turning in your lab

To turnin this project
0. Login to lore.

1. Place the required files in the directory lab1-src/

2. Go one directory above lab1-src and type:

        turnin -c cs354 -p lab1 lab1-src

3. Verify that your files have been turned in by typing:

        turnin -c cs354 -p lab1 -v

The deadline for lab1 is 11:59pm Tuesday September 4th. 



=======
||FAQ||
=======
>Some things are Labeled Elf32_Word or Elf64_Word in elf.h. For example 
>sh_name is the section header and is a Elf32_Word type. 
>These print out as strings in ElfDump, but right now they print out as 
>integers in my elfinfo. 
>When I try to print these out as strings using printf, I get a segfault. 
>My question is, how can I convert Elf32_Word and Elf54_Word to strings 
>so I can print them in printf? 
> 

1) Linker and libraries guide, page 190 : 

sh_name 
The name of the section. Its value is an index into the section header 
string table section giving the location of a null-terminated string. 
Section names and their descriptions are listed in Table 7-16. 

There is a 'section' which has the section names in string. Where is this 
section? 

2) Linker and libraries guide, page 183 : 

e_shstrndx 
The section header table index of the entry that is associated with the section name 
string table. If the file has no section name string table, this member holds the value 
SHN_UNDEF. 

Okay, it's stored at e_shstrndx in ELF header. Now, we know where is that section. 
Then, what's the structure of the section? 

3) In page 221, "String Table Section" is explained. It also gives an example of table 
and how to locate the string with index.

>When it prints symbol table information, there is a field called ver 
>that the man page of elfdump says is the index of the version dependency 
>of the symbol from the Version Definition Section. Do you know how to 
>get this information for each symbol? There doesn't seem to be anything 
>in the Elf32_Sym type about this. 

Please refer to the page 233 "Version Symbol Section" of the guide. It 
has the versioning information for each symbol in the symbol table.


>I am trying to run the code for the loader, and I keep getting an error 
>when I run the makefile. 
> 
>gcc -g -c hello.c 
>ld -dn -e mystart -M mapfile hello.o mylib.o -o hello 
>mapfile: file not recognized: File format not recognized 
>make: *** [hello] Error 1 
> 
>Thats the error, so I cant even load up the basic stuff for the loader. 
> Whats the deal? Thank you. 


See the output of 'which ld'. If it's not /usr/ccs/bin/ld, your path 
configuration is not correctly done. Modify your path (.bashrc or .cshrc) 
such that /usr/ccs/bin is prior to /p/gnu (or other path ld is chosen) or 
modify your Makefile such that /usr/ccs/bin/ld is used. 
------------------------------------------------------------------
>I have been working on the section header name part and and the symbol table
>since yesterday but cannot really understand how to go about it. I read the link
>provided in the lab handout but I still cannot understand how is the section   
>header name and the symbol table arranged in the object file. I would be really
>grateful to you if you could explain it to me and then I can work on the assignment.
>The object file will have multiple sections. Each section contains        
>a section header with the information about that section. The section
>header is represented by the Elf32_Shdr struct. See /usr/include/sys/elf.h

The section headers for the symbol table are the ones with the
field sh_type equal to SHT_SYMTAB (2) and SHT_DYNSYM(11). You need to
print those.

The sh_offset field in the section points to the position in the file
where the symbol table entries are stored. The entries come one after
another. The length of each entry is given by sh_ensize and you can
compute how many entries are with sh_size/sh_entsize.

The struct for the symbol table entries is given by struct ELF32_Sym.    
See /usr/include/sys/elf.h

The nt_name field is the offset in the string table for the symbol.

Before reading the symbol tables you will need to read the string table.
The section that corresponds to the string table is SHT_STRTAB (3).
Allocate with malloc enough memory to allocate sh_size bytes and read the
string table from sh_offset in the file. The string table is a bunch of   
strings one after the other of different sizes that are null terminated.  
Once you have allocated the string table in memory, you can use it.

For example, st_name in the symbol table is the string that starts in the
string table at the st_name position. Assuming that you allocated the
string table at char *strTab, then the name of the symbol will be at
strTab+sym->st_name.

See also the FAQ at the top of the handout.

------------------------------------------

>> what is the content in "ver" and "shndx"?
>> I read the manual and yet to find out what these two columns are corresponding to.

> The version strings are in the section
> "sect_headers[i].sh_type == SHT_SUNW_versym"


>> What about shndx?
>> 

It is the name of the section header where the symbol is defined:

sect_strings  + sect_headers[sym_tab[i].st_shndx].sh_name

---------------------------------------------------------
Please forward the below two answers to everyone in the class.

1. The version field in the Symbol tables can be obtained from SHT_SUNW_versym. The structure
is: Elf32_Versym.

2. Use the following mapping for printing the Others field in Symbol Table entries,
   0 -> 'D'
   1 -> 'I'
   2 -> 'H'
   3 -> 'P'


---------------------------------------------------------------