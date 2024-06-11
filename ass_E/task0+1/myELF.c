#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

# define buffLen 100
typedef struct {
  char *name;
  void (*fun)();
}fun_desc;

//--global vars---
int menu_size = 6;
int debug = 0;
int Currentfd = -1;
Elf32_Ehdr *header; 
Elf32_Shdr *symtab1;
Elf32_Shdr *symtab2;
int currPlace = 1;
int fd1 = -1;
int fd2 = -1;
char* currentFilenameOpen=NULL;
void* map_start;
void* map_start1;
void* map_start2; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */

//--function declerations---
Elf32_Shdr* getTable(char* _name);


void toggleDebugMode(){
    if (debug == 0) {
        printf("Debug flag now on\n");
        debug = 1;
  }
  else {
    printf("Debug flag now off\n");
    debug = 0;
  }
}

int LoadFile(){
    char filename[buffLen];
    int fd;

    fscanf(stdin,"%s",filename);
    if((fd = open(filename, O_RDWR)) < 0) {
      perror("error in open");
      exit(-1);
   }
    if(fstat(fd, &fd_stat) != 0 ) {
      perror("stat failed");
      exit(-1);
   }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
      perror("mmap failed");
      exit(-4);
   }
    if(Currentfd!=-1)
        close(Currentfd);
    Currentfd=fd;
    if(fd1 != -1){
        fd2 = Currentfd;
        map_start2 = map_start;
    }
    else{
        fd1 = Currentfd;
        map_start1 = map_start;
    }
	strcpy((char*)&currentFilenameOpen,(char*)filename);
    return Currentfd;
}


char* dataType(Elf32_Ehdr* header){
    switch (header->e_ident[5]){
    case ELFDATANONE:
        return "invalid data encoding";
        break;
    case ELFDATA2LSB:
        return "2's complement, little endian";
        break;
    case ELFDATA2MSB:
        return "2's complement, big endian";
        break;
    default:
        return "NO DATA";
        break;
    }
}
void examineFile(){
    printf("Enter file name: ");
    if(LoadFile()==-1)
        exit(1);
    header = (Elf32_Ehdr *) map_start;
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0){ //is elf file?
        printf("Magic:\t\t\t\t %X %X %X\n", header->e_ident[EI_MAG0],header->e_ident[EI_MAG1],header->e_ident[EI_MAG2]);
        printf("Data:\t\t\t\t %s\n",dataType(header));
        printf("Enty point address:\t\t 0x%x\n",header->e_entry);
        printf("Start of section headers:\t %d (bytes into file)\n",header->e_shoff);
        printf("Number of section headers:\t %d\n",  header->e_shnum);
        printf("Size of section headers:\t %d (bytes)\n",header->e_shentsize);
        printf("Start of program headers:\t %d (bytes into file)\n",header->e_phoff);
        printf("Number of program headers:\t %d\n",header->e_phnum);
        printf("Size of program headers:\t %d (bytes)\n",header->e_phentsize);
        if(currPlace == 1){
            symtab1 = getTable(".symtab");
            currPlace = 2;
        }else{
            symtab2 = getTable(".symtab");
            currPlace = 1;
        }

   }
    else{
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size); 
        close(Currentfd); 
        Currentfd = -1;
        currentFilenameOpen = NULL;
    }
}

char* sectionType(int value) {
    switch (value) {
        case SHT_NULL:return "NULL";
        case SHT_PROGBITS:return "PROGBITS";
        case SHT_SYMTAB:return "SYMTAB";
        case SHT_STRTAB:return "STRTAB";
        case SHT_RELA:return "RELA";
        case SHT_HASH:return "HASH";
        case SHT_DYNAMIC:return "DYNAMIC";
        case SHT_NOTE:return "NOTE";
        case SHT_NOBITS:return "NOBITS";
        case SHT_REL:return "REL";
        case SHT_SHLIB:return "SHLIB";
        case SHT_DYNSYM:return "DYNSYM";
        default:return "Unknown";
    }
}

void printSectionEntry(int index,char* name ,Elf32_Shdr* section,int offset){
    if(debug){
        printf("[%2d] %-18.18s\t%#06x\t%06d\t%06d\t%-13.10s\t%d\n",index, name ,section->sh_addr,section->sh_offset, section->sh_size, sectionType(section->sh_type),offset );
    }
    else{
        printf("[%2d] %-18.18s\t%#06x\t%06d\t%06d\t%-13.10s\n",index, name ,section->sh_addr,section->sh_offset, section->sh_size, sectionType(section->sh_type) );}
}

void printSectionNames(){
    if(Currentfd ==-1)
        perror("No file is currently open\n");
    else{
    	Elf32_Shdr* sections_table = map_start+header->e_shoff;
    	Elf32_Shdr* string_table_entry = map_start+header->e_shoff+(header->e_shstrndx*header->e_shentsize); //to get the names
		if(debug){
			fprintf(stderr,"section table address: %p\n",sections_table);
			fprintf(stderr,"string table entry: %p\n",string_table_entry);
            printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\t\toffset(bytes)\n");
        }
		else{printf("[Nr] Name\t\tAddr\t\tOff\tSize\tType\n");}
    	for (size_t i = 0; i < header->e_shnum; i++){       
    		Elf32_Shdr* entry = map_start+header->e_shoff+(i* header->e_shentsize);     //header->e_shoff+(i* header->e_shentsize) ==> section
        	char* name = map_start + string_table_entry->sh_offset + entry->sh_name;
        	printSectionEntry(i,name,entry,header->e_shoff+(i* header->e_shentsize));
    	}
  	}
}

Elf32_Shdr* getTable(char* _name){
    Elf32_Shdr* string_table_entry = map_start+ header->e_shoff+(header->e_shstrndx*header->e_shentsize);
    for (size_t i = 0; i < header->e_shnum; i++){
        Elf32_Shdr* entry = map_start+header->e_shoff+(i* header->e_shentsize);
        char* name = map_start + string_table_entry->sh_offset + entry->sh_name;
        if(strcmp(_name, name)==0){
            return entry;
        }
    }
    return NULL;
}
void printSymbols(){
    if (Currentfd == -1){
        perror("No file open!");
    }
    else{
        Elf32_Shdr *symbol_table_entry = getTable(".symtab");
        Elf32_Shdr *strtab = getTable(".strtab");       //get the entry of "name" --> symbol name
        Elf32_Shdr *shstrtab = getTable(".shstrtab");   // --> section name
        if (symbol_table_entry == NULL){
            perror("Symbol table not found!");
        }
        else {
            int entry_num = symbol_table_entry->sh_size / sizeof(Elf32_Sym);
            if(debug){
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\t\tsize\n");
            }
            else{
                printf("[Num]\tValue\t\tsection_index\tsection_name\t\tsymbol_name\n");
            }
            for (int i = 0; i < entry_num; i++)
            {
                Elf32_Sym *symb_entry = map_start + symbol_table_entry->sh_offset + (i * sizeof(Elf32_Sym));
                char *section_name;
                if (symb_entry->st_shndx == 0xFFF1) { section_name = "ABS";}
                else if (symb_entry->st_shndx == 0x0) { section_name = "UND";}
                else {
                    Elf32_Shdr *section_entry = map_start + header->e_shoff + (symb_entry->st_shndx * header->e_shentsize);
                    section_name = map_start + shstrtab->sh_offset + section_entry->sh_name;
                }

                char *symb_name = map_start + strtab->sh_offset + symb_entry->st_name;
                char *symb_size = map_start + strtab->sh_offset + symb_entry->st_size;

                if (debug){
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t%-20.30s\t\t%-20.30s\n",
                    i, symb_entry->st_value, symb_entry->st_shndx, section_name,symb_name,symb_size);
                }
                else {
                    printf("[%2d]\t%#09x\t%d\t\t%-13.20s\t\t%-20.30s\n",
                        i, symb_entry->st_value, symb_entry->st_shndx, section_name,symb_name);
                }
            }
            
        }
    }
}


void checkMerge() {
  if (symtab1 == NULL || symtab2 == NULL) {
    printf("Error: 2 ELF files have not been opened and mapped\n");
    return;
  }

  if (symtab1->sh_entsize != symtab2->sh_entsize) {
    printf("Error: feature not supported\n");
    return;
  }

  Elf32_Sym* sym1 = (Elf32_Sym*)((char*)map_start1 + symtab1->sh_offset + symtab1->sh_entsize);
  Elf32_Sym* sym2 = (Elf32_Sym*)((char*)map_start2 + symtab2->sh_offset + symtab2->sh_entsize);
  for (int i = 1; i < symtab1->sh_size; i++) {
    if(sym1->st_shndx != 0){
    if (sym1->st_shndx == STN_UNDEF) {
      if (sym2->st_shndx == STN_UNDEF) {
        printf("Symbol %d undefined\n", sym1->st_name);
      }
    } else {
      if (sym2->st_shndx != STN_UNDEF) {
        printf("Symbol %d multiply defined\n", sym1->st_name);
      }
    }
    sym1 = (Elf32_Sym*)((char*)sym1 + symtab1->sh_entsize);
    sym2 = (Elf32_Sym*)((char*)sym2 + symtab2->sh_entsize);
  }
  }
}

void quit(){
    if (debug) { printf("quitting\n");}
    exit(0);
}

void printMenu (fun_desc menu[]){
    printf("Please choose a function:\n");
    for(int i = 0; i < menu_size; i++)
        printf("%d) %s\n", i, menu[i].name);
    printf("Option: ");
}

int getOption (){
  int op;
  scanf("%d", &op);
  if (op >= 0 && op < menu_size){
    printf("\nwhithin bounds \n" );
    return op;
  }
  else{
    printf("\nNot within bounds\n" );
    return -1;
  }
}

int main(int argc, char **argv){
  fun_desc menu[] = {{"Toggle Debug Mode",toggleDebugMode},{"Examine ELF File",examineFile},
                    {"Print Section Names",printSectionNames},{"Print Symbols",printSymbols},
                    {"Check files for merge",checkMerge},{"Quit",quit},{NULL,NULL}};
  size_t index=0;
  while (menu[index].name != NULL){index = index +1; }
  while (1) {
    printMenu(menu);
    int option = getOption();
    if (option != -1) { menu[option].fun(); }
    printf("\n");
  }
return 0;
}