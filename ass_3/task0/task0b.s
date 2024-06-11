section .data
    hello: db 'Hello, World!', 10
section .text
    global _start
_start:
    ; write the string to stdout
    mov eax, 4       ; system call number for write
    mov ebx, 1       ; file descriptor for stdout
    mov ecx, hello   ; pointer to the string to write
    mov edx, 14      ; length of the string
    int 0x80         ; call the kernel to perform the system call

    ; exit the program
    mov eax, 1       ; system call number for exit
    xor ebx, ebx     ; return value for exit (0)
    int 0x80         ; call the kernel to perform the system call
