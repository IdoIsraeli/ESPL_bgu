#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
/*
//Write a program, which gets a single command line argument.
//The argument will be the file name of a 32bit ELF formatted executable.
int foreach_phdr(void *map_start,void (*func)(Elf32_Phdr *,int), int arg){
    Elf32_Ehdr *header = (Elf32_Ehdr *)map_start;
    Elf32_Phdr *phdr = (Elf32_Phdr *)(map_start + header->e_phoff);
    int i;
    for(i=0;i<header->e_phnum;i++){
        func(phdr,arg);
        phdr++;
    }
    return 0;
}*/

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

    const char *flags_str = "";
    if (phdr->p_flags & PF_R)
        flags_str = "R";
    if (phdr->p_flags & PF_W)
        flags_str = "RW";
    if (phdr->p_flags & PF_X)
        flags_str = "R E";

    printf("%-10s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x %-4s 0x%x\n",
           type_str, phdr->p_offset, phdr->p_vaddr, phdr->p_paddr,
           phdr->p_filesz, phdr->p_memsz, flags_str, phdr->p_align);
}



// Function to be applied to each program header which prints the address of each program header
void process_phdr0(Elf32_Phdr *phdr, int arg) {
    static int header_count = 0;
    printf("Program header number %d at address %p\n", header_count, phdr);
    header_count++;
}

int main(int argc, char *argv[]) {
    const char *file_name = argv[1];

    // Open the file
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return 1;
    }

    // Get the file size
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Failed to get file size");
        close(fd);
        return 1;
    }

    // Map the file to memory
    void *map_start = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map_start == MAP_FAILED) {
        perror("Failed to map file");
        close(fd);
        return 1;
    }

    foreach_phdr(map_start, process_phdr0, 0);
    munmap(map_start, st.st_size);
    close(fd);

    return 0;
}