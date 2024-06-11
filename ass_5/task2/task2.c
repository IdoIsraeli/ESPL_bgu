#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

int convertmmapflag(int flg);
void process_phdr1(Elf32_Phdr *phdr, int arg);
char* convertFlag(int flg);
extern int startup(int argc, char **argv, void (*start)());
Elf32_Ehdr *header;

void load_phdr(Elf32_Phdr *phdr, int fd){
    if(phdr->p_type != PT_LOAD)
        return;
    void *vadd = (void *)(phdr -> p_vaddr&0xfffff000);
    int offset = phdr -> p_offset&0xfffff000;
    int padding = phdr -> p_vaddr & 0xfff;
    void* map_start = mmap(vadd, phdr->p_memsz+padding, convertmmapflag(phdr->p_flags) , MAP_FIXED | MAP_PRIVATE, fd, offset);
    if (map_start == MAP_FAILED ) {
        perror("mmap failed");
        exit(-4);
    }
    process_phdr1(phdr,0);
}


int foreach_phdr(void *map_start,void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr* header = (Elf32_Ehdr*)map_start;
    for (size_t i = 0; i < header->e_phnum; i++){
        Elf32_Phdr* entry = map_start+header->e_phoff+(i* header->e_phentsize);
        func(entry,arg);
    }
    return 0;
}
// Function to be applied to each program header that mimics that cmdLine "readelf -l <file_name>"
void process_phdr1(Elf32_Phdr *phdr, int arg) {
    const char *type_str;
    switch (phdr->p_type) {
        case PT_NULL:
            type_str = "NULL";
            break;
        case PT_LOAD:
            type_str = "LOAD";
            break;
        case PT_DYNAMIC:
            type_str = "DYNAMIC";
            break;
        case PT_INTERP:
            type_str = "INTERP";
            break;
        case PT_NOTE:
            type_str = "NOTE";
            break;
        case PT_SHLIB:
            type_str = "SHLIB";
            break;
        case PT_PHDR:
            type_str = "PHDR";
            break;
        case PT_TLS:
            type_str = "TLS";
            break;
        default:
            type_str = "UNKNOWN";
            break;
    }
    printf("%-10s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x %s 0x%x\n",
           type_str, phdr->p_offset, phdr->p_vaddr, phdr->p_paddr,
           phdr->p_filesz, phdr->p_memsz, convertFlag(phdr->p_flags), phdr->p_align);
}


char* convertFlag(int flg){
    switch (flg){
        case 0x000: return "";
        case 0x001: return "E";
        case 0x002: return "W";
        case 0x003: return "WE";
        case 0x004: return "R";
        case 0x005: return "RE";
        case 0x006: return "RW";
        case 0x007: return "RWE";
        default:return "Unknown";
    }
}

int convertmmapflag(int flg){
    switch (flg){
        case 0x000: return 0;
        case 0x001: return PROT_READ;
        case 0x002: return PROT_WRITE;
        case 0x003: return PROT_READ | PROT_WRITE;
        case 0x004: return PROT_EXEC;
        case 0x005: return PROT_READ | PROT_EXEC;
        case 0x006: return PROT_EXEC | PROT_WRITE;
        case 0x007: return PROT_READ | PROT_WRITE | PROT_EXEC;
        default:return -1;
    }
}



int main(int argc, char *argv[]) {
    const char *file_name = argv[1];
    if(argc<2){
        perror("not enough arguments");
        exit(1);
    }
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }
    struct stat st; // Get the file size
    if (fstat(fd, &st) == -1) {
        perror("Failed to get file size");
        close(fd);
        return 1;
    }
    void *map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("Failed to map file");
        close(fd);
        return 1;
    }
    header = (Elf32_Ehdr *) map_start;
    foreach_phdr(map_start, load_phdr, fd);
    startup(argc-1, argv+1, (void *)(header->e_entry));
    munmap(map_start, st.st_size);
    close(fd);
    return 0;
}