
#include <elf.h>
#include <stdio.h>

main(argc, argv)
     int argc;
     char **argv;
{
	FILE *fp;
	Elf32_Ehdr ehdr;
	Elf32_Phdr phdr;
	Elf32_Shdr shdr;
	int i;
	
	if (argc != 2) {
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		exit(1);
	}

	fp = fopen(argv[1], "r");

	if (fp == 0) {
		fprintf(stderr, "%s: can't open %s\n",argv[0], argv[1]);
 		exit(1);
	}

	if ( fread( &ehdr, sizeof(ehdr), 1, fp ) != 1 ) {
		fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
		exit(1);
	}
	
	printf("ELF Header\n");
	printf("  ei_magic:   { 0x%x, %c, %c, %c }\n", ehdr.e_ident[0], ehdr.e_ident[1], ehdr.e_ident[2], ehdr.e_ident[3]);
	
	printf("  ei_class:   ");
	switch(ehdr.e_ident[4]){
	  case 0: printf("ELFCLASSNONE\t");
	    break;
	  case 1: printf("ELFCLASS32\t");
	    break;
	  case 2: printf("ELFCLASS64\t");
	    break;
	  case 3: printf("ELFCLASSNUM\t");
	    break;
	}
	
	printf("  ei_data:       ");
	switch(ehdr.e_ident[5]){
	  case 0: printf("ELFDATANONE\n");
	    break;
	  case 1: printf("ELFDATA2LSB\n");
	    break;
	  case 2: printf("ELFDATA2MSB\n");
	    break;
	  case 3: printf("ELFDATANUM\n");
	    break;
	}
	
	printf("  ei_osabi:   ");
	switch(ehdr.e_ident[7]){
	  case 0: printf("ELFOSABI_NONE\t");
	    break;
	  case 1: printf("ELFOSABI_HPUX\t");
	    break;
	  case 2: printf("ELFOSABI_NETBSD\t");
	    break;
	  case 3: printf("ELFOSABI_LINUX\t");
	    break;
	  case 4: printf("ELFOSABI_UNKNOWN4\t");
	    break;
	  case 5: printf("ELFOSABI_UNKNOWN5\t");
	    break;  
	  case 6: printf("ELFOSABI_SOLARIS\t");
	    break;
	  case 7: printf("ELFOSABI_AIX\t");
	    break;
	  case 8: printf("ELFOSABI_IRIX\t");
	    break;
	  case 9: printf("ELFOSABI_FREEBSD\t");
	    break;
	  case 10: printf("ELFOSABI_TRU64\t");
	    break;
	  case 11: printf("ELFOSABI_MODESTO\t");
	    break;
	  case 12: printf("ELFOSABI_OPENBSD\t");
	    break;
	  case 13: printf("ELFOSABI_OPENVMS\t");
	    break;
	  case 14: printf("ELFOSABI_NSK\t");
	    break;
	  case 15: printf("ELFOSABI_AROS\t");
	    break;
	  case 97: printf("ELFOSABI_ARM\t");
	    break;
	  case 255: printf("ELFOSABI_STANDALONE\t");
	    break;
	}
	
	printf("  ei_abiversion: ");
	if(ehdr.e_ident[8] == 0){
	  printf("EAV_SUNW_NONE\n");
	} else if(ehdr.e_ident[8] == 1) {
	  printf("EAV_SUNW_CURRENT\n");
	} else if(ehdr.e_ident[8] == 2) {
	  printf("EAV_SUNW_NUM\n");
	}
	
	printf("  e_machine:  ");
	switch(ehdr.e_machine){
	  case 0: printf("EM_NONE\t\t");
	    break;
	  case 1: printf("EM_M32\t\t");
	    break;
	  case 2: printf("EM_SPARC\t\t");
	    break;
	  case 3: printf("EM_386\t\t");
	    break;
	  case 4: printf("EM_68K\t\t");
	    break;
	  case 5: printf("EM_88K\t\t");
	    break;
	  case 6: printf("EM_486\t\t");
	    break;
	  case 7: printf("EM_860\t\t");
	    break;
	  case 8: printf("EM_MIPS\t\t");
	    break;
	  case 9: printf("EM_S370\t\t");
	    break;
	  case 10: printf("EM_MIPS_RS3_LE\t");
	    break;
	  case 11: printf("EM_RS6000\t\t");
	    break;
	  case 15: printf("EM_PA_RISC\t\t");
	    break;
	  case 16: printf("EM_nCube\t\t");
	    break;
	  case 17: printf("EM_VPP500\t\t");
	    break;
	  case 18: printf("EM_SPARC32PLUS\t");
	    break;
	  
	}
	
	printf("  e_version:     ");
	if(ehdr.e_version == 0){
	  printf("EV_NONE\n");
	} else if(ehdr.e_version == 1){
	  printf("EV_CURRENT\n");
	} else if(ehdr.e_version == 2){
	  printf("EV_NUM\n");
	}
	
	printf("  e_type:     ");
	switch(ehdr.e_type){
	  case 0 : printf("ET_NONE\n");
	    break;
	  case 1 : printf("ET_REL\n");
	    break;
	  case 2 : printf("ET_EXEC\n");
	    break;
	  case 3 : printf("ET_DYN\n");
	    break;
	  case 4 : printf("ET_CORE\n");
	    break;
	  case 5 : printf("ET_NUM\n");
	    break;
	  case 0xfe00 : printf("ET_LOOS\n");
	    break;
	  case 0xfeff : printf("ET_LOSUNW\n");
	    break;
	  case 0xff00 : printf("ET_LOPROC\n");
	    break;
	  case 0xffff : printf("ET_HIPROC\n");
	    break;
	}
	
	printf("  e_flags:    ");
	if(ehdr.e_flags == 256)
	  printf("\t\t[ EF_SPARC_32PLUS ]\n");
	else
	  printf("\t\t%d\n", ehdr.e_flags);
	
	printf("  e_entry:    ");
	printf("\t\t0x%x\t", ehdr.e_entry);
	printf("  e_ehsize:    ");
	printf("%d  ", ehdr.e_ehsize);
	printf("  e_shstrndx:  ");
	printf("%d\n", ehdr.e_shstrndx);
	
	printf("  e_shoff:     ");
	printf("\t\t0x%x\t", ehdr.e_shoff);
	printf("  e_shentsize: ");
	printf("%d  ", ehdr.e_shentsize);
	printf("  e_shnum:     ");
	printf("%d\n", ehdr.e_shnum);
	
	printf("  e_phoff:     ");
	printf("\t\t0x%x\t", ehdr.e_phoff);
	printf("  e_phentsize: ");
	printf("%d  ", ehdr.e_phentsize);
	printf("  e_phnum:     ");
	printf("%d\n", ehdr.e_phnum);	
	
	printf("\n");
	
	//==============================================================================================================================
	//Program Header
	for ( i = 0; i < ehdr.e_phnum; i++ ) {
	  printf("Program Header[%d]:\n", i);
	  fseek( fp, ehdr.e_phoff + i * ehdr.e_phentsize, SEEK_SET );
	  if ( fread( &phdr, sizeof(phdr), 1, fp ) != 1 ) {
	    fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	    exit(1);
	  }
	  printf("\tp_vaddr:\t0x%x", phdr.p_vaddr);
	  printf("\tp_flags:\t");
	  switch(phdr.p_flags){
	    case 0 : printf("0");
	      break;
	    case 0x1 : printf("[ PF_X ]");
	      break;
	    case 0x2 : printf("[ PF_W ]");
	      break;
	    case 0x4 : printf("[ PF_R ]");
	      break;
	    case 0x5 : printf("[ PF_X PF_R ]");
	      break;
	    case 0x6 : printf("[ PF_W PF_R ]");
	      break;
	    case 0x7 : printf("[ PF_X PF_W PF_R ]");
	      break;
	  }
	  printf("\n");
	  printf("\tp_paddr:\t%d", phdr.p_paddr);
	  printf("\tp_type:\t\t");
	  switch(phdr.p_type){
	    case 0: printf("[ PT_NULL ]\n");
	      break;
	    case 1: printf("[ PT_LOAD ]\n");
	      break;
	    case 2: printf("[ PT_DYNAMIC ]\n");
	      break;
	    case 3: printf("[ PT_INTERP ]\n");
	      break;
	    case 4: printf("[ PT_NOTE ]\n");
	      break;
	    case 5: printf("[ PT_SHLIB ]\n");
	      break;
	    case 6: printf("[ PT_PHDR ]\n");
	      break;
	    case 7: printf("[ PT_TLS ]\n");
	      break;
	    case 8: printf("[ PT_NUM ]\n");
	      break;
	    case 0x60000000: printf("[ PT_LOOS ]\n");
	      break;
	    case 0x6464e550: printf("[ PT_SUNW_UNWIND ]\n");
	      break;
	    case 0x6474e550: printf("[ PT_SUNW_EH_FRAME ]\n");
	      break;
	    case 0x6474e551: printf("[ PT_GNU_STACK ]\n");
	      break;
	    case 0x6474e552: printf("[ PT_GNU_RELRO ]\n");
	      break;
	    case 0x6ffffffb: printf("[ PT_SUNWSTACK ]\n");
	      break;
	    case 0x6ffffffc: printf("[ PT_SUNWDTRACE ]\n");
	      break;
	    case 0x6ffffffd: printf("[ PT_SUNWCAP ]\n");
	      break;
	    case 0x6fffffff: printf("[ PT_HISUNW ]\n");
	      break;
	    case 0x70000000: printf("[ PT_LOPROC ]\n");
	      break;
	    case 0x7fffffff: printf("[ PT_HIPROC ]\n");
	      break;
	  }
	  printf("\tp_filesz:\t0x%x", phdr.p_filesz);
	  if(phdr.p_memsz == 0x0)
	    printf("\tp_memsz:\t0\n");
	  else
	    printf("\tp_memsz:\t0x%x\n", phdr.p_memsz);
	  if(phdr.p_offset == 0x0)
	    printf("\tp_offset:\t0");
	  else
	    printf("\tp_offset:\t0x%x", phdr.p_offset);
	  if(phdr.p_align == 0x0)
	    printf("\tp_align:\t0\n");
	  else
	    printf("\tp_align:\t0x%x\n", phdr.p_align);
	  printf("\n");
	}
	
	//==============================================================================================================================
	//Section Header
	Elf32_Shdr shdrTemp;
	Elf32_Off tableOffset;
	Elf32_Word tableSize = 0;
	Elf32_Word tableEntrySize = 0;
	int check = 0;
	Elf32_Off tableOffset2;
	Elf32_Word tableSize2 = 0;
	Elf32_Word tableEntrySize2 = 0;
	int check2 = 0;
	
	
	fseek( fp, ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize, SEEK_SET );
	if ( fread( &shdrTemp, sizeof(shdrTemp), 1, fp ) != 1 ) {
	  fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	  exit(1);
	}
	char *holder = (char*) malloc (sizeof(shdrTemp).sh_size);
	fseek(fp, shdrTemp.sh_offset, SEEK_SET);
	fread(holder, shdrTemp.sh_size, 1, fp);
	
	
	for ( i = 1; i < ehdr.e_shnum; i++ ) {
	  printf("Section Header[%d]:\t", i);
	  fseek( fp, ehdr.e_shoff + i * ehdr.e_shentsize, SEEK_SET );
	  if ( fread( &shdr, sizeof(shdr), 1, fp ) != 1 ) {
	    fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	    exit(1);
	  }
	  
	  printf("sh_name:   %s\n", holder + shdr.sh_name);
	  //GETTING ADDRESS FOR MEM ADDRESS
	  if(strcmp((holder + shdr.sh_name), ".dynsym") == 0) {
	    check = 1;
	    tableOffset = shdr.sh_offset;
	    tableSize = shdr.sh_size/shdr.sh_entsize;
	    tableEntrySize = shdr.sh_entsize;
	  } else if(strcmp((holder + shdr.sh_name), ".symtab") == 0) {
	    check2 = 1;
	    tableOffset2 = shdr.sh_offset;
	    tableSize2 = shdr.sh_size/shdr.sh_entsize;
	    tableEntrySize2 = shdr.sh_entsize;
	  }
	  
	  if(shdr.sh_addr == 0x0)
	    printf("\tsh_addr:\t0");
	  else
	    printf("\tsh_addr:\t0x%5x", shdr.sh_addr);
	  printf("\t\tsh_flags:\t");
	  /*if(shdr.sh_flags == 0x0)
	    printf("0");
	  else {
	    printf("[");
	    while(shdr.sh_flags > 0x0) {
	      if(shdr.sh_flags > 0xf0000000) {
		shdr.sh_flags -= 0xf0000000;
		printf(" SHF_MASKPROC ");
	      } else if(shdr.sh_flags > 0x0ff00000) {
		shdr.sh_flags -= 0x0ff00000;
		printf(" SHF_MASKOS ");
	      } else if(shdr.sh_flags > 0x400) {
		shdr.sh_flags -= 0x400;
		printf(" SHF_TLS ");
	      } else if(shdr.sh_flags > 0x200) {
		shdr.sh_flags -= 0x200;
		printf(" SHF_GROUP ");
	      } else if(shdr.sh_flags > 0x100) {
		shdr.sh_flags -= 0x100;
		printf(" SHF_OS_NONCONFORMING ");
	      } else if(shdr.sh_flags > 0x80) {
		shdr.sh_flags -= 0x80;
		printf(" SHF_LINK_ORDER ");
	      } else if(shdr.sh_flags > 0x40) {
		shdr.sh_flags -= 0x40;
		printf(" SHF_INFO_LINK ");
	      } else if(shdr.sh_flags > 0x20) {
		shdr.sh_flags -= 0x20;
		printf(" SHF_STRINGS ");
	      } else if(shdr.sh_flags > 0x10) {
		shdr.sh_flags -= 0x10;
		printf(" SHF_MERGE ");
	      } else if(shdr.sh_flags > 0x4) {
		shdr.sh_flags -= 0x4;
		printf(" SHF_EXECINSTR ");
	      } else if(shdr.sh_flags > 0x2) {
		shdr.sh_flags -= 0x2;
		printf(" SHF_ALLOC ");
	      } else if(shdr.sh_flags > 0x1) {
		shdr.sh_flags -= 0x1;
		printf(" SHF_WRITE ");
	      }
	    }
	    printf("]");
	  }*/
	  
	  switch(shdr.sh_flags){
	    case 0 : printf("0");
	      break;
	    case 0x01: printf("[ SHF_WRITE ]");
	      break;
	    case 0x02: printf("[ SHF_ALLOC ]");
	      break;
	    case 0x03: printf("[ SHF_WRITE SHF_ALLOC ]");
	      break;
	    case 0x04: printf("[ SHF_EXECINSTR ]");
	      break;
	    case 0x06: printf("[ SHF_ALLOC SHF_EXECINSTR ]");
	      break;
	    case 0x07: printf("[ SHF_WRITE SHF_ALLOC SHF_EXECINSTR ]");
	      break;
	    case 0x10: printf("[ SHF_MERGE ]");
	      break;
	    case 0x20: printf("[ SHF_STRINGS ]");
	      break;
	    case 0x22: printf("[ SHF_ALLOC SHF_STRINGS ]");
	      break;
	    case 0x30: printf("[ SHF_MERGE SHF_STRINGS ]");
	      break;
	    case 0x40: printf("[ SHF_INFO_LINK ]");
	      break;
	    case 0x42: printf("[ SHF_ALLOC SHF_INFO_LINK ]");
	      break;
	    case 0x44: printf("[ SHF_INFO_LINK SHF_EXECINSTR ]");
	      break;
	    case 0x80: printf("[ SHF_LINK_ORDER ]");
	      break;
	    case 0x100: printf("[ SHF_OS_NONCONFORMING ]");
	      break;
	    case 0x200: printf("[ SHF_GROUP ]");
	      break; 
	    case 0x400: printf("[ SHF_TLS ]");
	      break; 
	    case 0x0ff00000: printf("[ SHF_MASKOS ]");
	      break;
	    case 0xf0000000: printf("[ SHF_MASKPROC ]");
	      break;
	    default : printf("0x%x", shdr.sh_flags);
	  }
	  
	  printf("\n");
	  printf("\tsh_size:\t0x%x", shdr.sh_size);
	  printf("\t\tsh_type:\t");
	  switch(shdr.sh_type){
	    case 0 : printf("[ SHT_NULL ]");
	      break;
	    case 1 : printf("[ SHT_PROGBITS ]");
	      break;
	    case 2 : printf("[ SHT_SYMTAB ]");
	      break;
	    case 3 : printf("[ SHT_STRTAB ]");
	      break;
	    case 4 : printf("[ SHT_RELA ]");
	      break;
	    case 5 : printf("[ SHT_HASH ]");
	      break;
	    case 6 : printf("[ SHT_DYNAMIC ]");
	      break;
	    case 7 : printf("[ SHT_NOTE ]");
	      break;
	    case 8 : printf("[ SHT_NOBITS ]");
	      break;
	    case 9 : printf("[ SHT_REL ]");
	      break;
	    case 10 : printf("[ SHT_SHLIB ]");
	      break;
	    case 11 : printf("[ SHT_DYNSYM ]");
	      break;
	    case 12 : printf("[ SHT_UNKNOWN12 ]");
	      break;
	    case 13 : printf("[ SHT_UNKNOWN13 ]");
	      break;
	    case 14 : printf("[ SHT_INIT_ARRAY ]");
	      break;
	    case 15 : printf("[ SHT_FINI_ARRAY ]");
	      break;
	    case 16 : printf("[ SHT_PREINIT_ARRAY ]");
	      break;
	    case 17 : printf("[ SHT_GROUP ]");
	      break;
	    case 18 : printf("[ SHT_SYMTAB_SHNDX ]");
	      break;
	    case 19 : printf("[ SHT_NUM ]");
	      break;
	    case 0x60000000: printf("[ SHT_LOOS ]");
	      break;
	    case 0x6ffffffe: printf("[ SHT_SUNW_verneed ]");
	      break;
	    case 0x6fffffff: printf("[ SHT_SUNW_versym ]");
	      break;
	    default : printf("%d", shdr.sh_type);
	  }
	  printf("\n\tsh_offset:\t0x%x", shdr.sh_offset);
	  if(shdr.sh_entsize == 0x0)
	    printf("\t\tsh_entsize:\t%d\n", shdr.sh_entsize);
	  else
	    printf("\tsh_entsize:\t0x%x (%d entries)\n", shdr.sh_entsize, shdr.sh_size/shdr.sh_entsize);
	  printf("\tsh_link:\t%d", shdr.sh_link);
	  if(shdr.sh_info == 0x0)
	    printf("\t\tsh_info:\t%d\n", shdr.sh_info);
	  else
	    printf("\tsh_info:\t%d\n", shdr.sh_info);
	  printf("\tsh_addralign:\t0x%x\n", shdr.sh_addralign);
	  printf("\n");
	}
	
	//==============================================================================================================================
	//Symbol Table dynsym
	Elf32_Sym sym;
	
	printf("Symbol Table Section:\t.dynsym\n");
	if(check == 1){
	  printf("\tindex\t   value\tsize\ttype\tbind   oth  ver\tshndx\t\tname\n");
	  for(i = 0 ; i < tableSize ; i++) {
	    fseek( fp, tableOffset + i *  tableEntrySize, SEEK_SET );
	    if ( fread( &sym, sizeof(sym), 1, fp ) != 1 ) {
	      fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	      exit(1);
	    }
	    printf("\t[%d]", i);
	    printf("\t0x%.8x", sym.st_value);
	    printf("   0x%.8x", sym.st_size);
	    printf("\t");
	    switch(sym.st_info & 0xf){
	      case 0:
		printf("NOTY");
		break;
	      case 1:
		printf("OBJT");
		break;
	      case 2:
		printf("FUNC");
		break;
	      case 3:
		printf("SECT");
		break;
	      case 4:
		printf("FILE");
		break;
	      case 5:
		printf("COMM");
		break;
	      case 6:
		printf("TLS");
		break;
	      case 7:
		printf("NUM");
		break;
	      case 10:
		printf("LOOS");
		break;
	      case 12:
		printf("HIOS");
		break;
	      case 13:
		printf("LOPR");
		break;
	      case 15:
		printf("HIPR");
		break;
	    }
	    printf("    ");
	    switch(sym.st_info >> 4){
	      case 0:
		printf("LOCL");
		break;
	      case 1:
		printf("GLOB");
		break;
	      case 2:
		printf("WEAK");
		break;
	      case 3:
		printf("NUM");
		break;
	    }
	    printf("    ");
	    switch(sym.st_other & 0x7) {
	      case 0:
		printf("D");
		break;
	      case 1:
		printf("I");
		break;
	      case 2:
		printf("H");
		break;
	      case 3:
		printf("P");
		break;
	    }
	    printf("    ");
	    printf("%d", shdr.sh_type); // CHECK HOW TO PRINT VERSION # ------------------------------------------
	    printf("\t");
	    
	    fseek( fp, ehdr.e_shoff + sym.st_shndx * ehdr.e_shentsize, SEEK_SET );
	    if ( fread( &shdr, sizeof(shdr), 1, fp ) != 1 ) {
	      fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	      exit(1);
	    }
	    
	    if(!sym.st_shndx)
	      printf("UNDEF");
	    else 
	      printf("%s", holder + shdr.sh_name);
	    

	    char *tempTable = (char *) malloc (tableSize); // CHECK HOW TO PRINT NAME!!!!!!!!!!!!!!!!!!!!!!!!!
	    fseek( fp, tableOffset, SEEK_SET );
	    if ( fread( tempTable, tableSize, 1, fp ) != 1 ) {
	      fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	      exit(1);
	    }
	    printf("\t\t");
	    if(sym.st_name != NULL)
	      printf("%s", tempTable + sym.st_name);
	    printf("\n");
	  }
	}

	printf("\n");
	
	//==============================================================================================================================
	//Symbol Table symtab
	Elf32_Sym sym2;
	FILE *fp2;
	
	printf("Symbol Table Section:\t.symtab\n");
	if(check2 == 1){
	  printf("\tindex\t   value\tsize\ttype\tbind   oth  ver\tshndx\t\tname\n");
	  for(i = 0 ; i < tableSize2 ; i++) {
	    fseek( fp, tableOffset2 + i *  tableEntrySize2, SEEK_SET );
	    if ( fread( &sym2, sizeof(sym2), 1, fp ) != 1 ) {
	      fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	      exit(1);
	    }
	    printf("\t[%d]", i);
	    printf("\t0x%.8x", sym2.st_value);
	    printf("   0x%.8x", sym2.st_size);
	    printf("\t");
	    switch(sym2.st_info & 0xf){
	      case 0:
		printf("NOTY");
		break;
	      case 1:
		printf("OBJT");
		break;
	      case 2:
		printf("FUNC");
		break;
	      case 3:
		printf("SECT");
		break;
	      case 4:
		printf("FILE");
		break;
	      case 5:
		printf("COMM");
		break;
	      case 6:
		printf("TLS");
		break;
	      case 7:
		printf("NUM");
		break;
	      case 10:
		printf("LOOS");
		break;
	      case 12:
		printf("HIOS");
		break;
	      case 13:
		printf("LOPR");
		break;
	      case 15:
		printf("HIPR");
		break;
	    }
	    printf("    ");
	    switch(sym2.st_info >> 4){
	      case 0:
		printf("LOCL");
		break;
	      case 1:
		printf("GLOB");
		break;
	      case 2:
		printf("WEAK");
		break;
	      case 3:
		printf("NUM");
		break;
	    }
	    printf("    ");
	    switch(sym2.st_other & 0x7) {
	      case 0:
		printf("D");
		break;
	      case 1:
		printf("I");
		break;
	      case 2:
		printf("H");
		break;
	      case 3:
		printf("P");
		break;
	    }
	    printf("    ");
	    printf("0");
	    printf("\t");
	    
	    if(!sym2.st_shndx)
	      printf("UNDEF");
	    else if(sym2.st_shndx == 65521)
	      printf("ABS");
	    else {
	      fseek( fp, ehdr.e_shoff + sym2.st_shndx * ehdr.e_shentsize, SEEK_SET );
	      if ( fread( &shdr, sizeof(shdr), 1, fp ) != 1 ) {
		fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
		exit(1);
	      }
	      printf("%s", holder + shdr.sh_name);
	    }
	    
	    /*
	    char *tempTable = (char *) malloc (tableSize);
	    fseek( fp, ehdr.e_shoff + sym.st_shndx * ehdr.e_shentsize, SEEK_SET );
	    if ( fread( tempTable, tableSize, 1, fp ) != 1 ) {
	      fprintf(stderr, "%s: couldn't read %s\n", argv[0], argv[1]);
	      exit(1);
	    }
		
	    printf("\t");
	    if(sym2.st_name == NULL)
	      printf("%s", tempTable + shdr.sh_name);*/
	    if(i == 2)
	      printf("\t\t%s", argv[1]);
	    printf("\n");
	  }
	}
	
	exit(0);
}
