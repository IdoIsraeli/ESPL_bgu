section .data
        infile: dd STDIN
        outfile: dd STDOUT
        STDIN EQU 0
        STDOUT EQU 1
        STDERR EQU 2
        READ EQU 3
        WRITE EQU 4
        OPEN EQU 5
        CLOSE EQU 6
        READONLY EQU 0
        CREATE EQU 64
        WRITEONLY EQU 1

global _start
global system_call
global main
global my_puts
global encoder
global closeFile


section .bss
        buff: resb 100
        file_name: resb 4


        
section .rodata
        BACKSPACE: db 10
        
section .text

_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller


main:
        push ebp
        mov ebp, esp
        mov ecx, [ebp+8]         ; argc pointer
        mov edx, [ebp+12]        ; argv
        mov ebx, 0
next:   mov eax ,[edx+ebx*4]     ;now holds argv[i]
        cmp word [eax], "-i"     ;check if there's inputFile
        jnz NO_Input_File
        add eax,2                ; skip the "-i"
        mov dword [file_name], eax  
openInput:
        mov eax, OPEN
        mov ebx, [file_name]
        mov ecx, READONLY
        mov edx, 0777
        int 0x80
        mov [infile], eax
        jmp Done_Flags                

NO_Input_File:
        cmp word[eax], "-o"
        jnz Done_Flags
        add eax,2
        mov dword [file_name], eax

Create_File_for_Output:
        mov eax, OPEN
        mov ebx, dword [file_name]
        mov ecx, CREATE+WRITEONLY
        mov edx, 0777
        int 0x80
        mov [outfile], eax

Done_Flags:
        mov esi, [infile]
        mov edi, [outfile]

continueAfterRediractionChecks:
        push dword STDERR
        push dword eax           ;holds the arguments
        call my_puts             ;to print the word
        add esp, 8
        push dword STDERR
        push dword BACKSPACE ; add to the stack str for my_strlen call
        call my_puts ; 
        add esp, 8
        inc ebx
        cmp ebx, ecx
        jnz next
        call encoder
        mov eax,[infile]
        cmp eax, STDIN
        jz coninueStam
        push dword eax
        call closeFile
        add esp, 4
coninueStam:
        mov eax, [outfile]
        cmp eax, STDOUT
        jz normalExit
        push dword eax
        call closeFile
        add esp, 4
normalExit:
        mov esp, ebp
        pop ebp
        mov eax, 0
        ret
error:
        mov eax,[infile]
        cmp eax,STDIN
        jz conprob
        push dword eax
        call closeFile
        add esp, 4
conprob:
        mov eax, [outfile]
        cmp eax, STDOUT
        jz errorExit
        push dword eax
        call closeFile
        add esp, 4
errorExit:
        mov esp, ebp
        pop ebp
        mov eax, 1
        ret


my_strlen: 
        mov eax, 1         ;eax holds the string's length
cont:   cmp byte [ecx], 0  ;ecx is the pointer to the string
        jz done
        inc ecx
        inc eax
        jmp cont
done:   ret

my_puts: push ebp
         mov ebp, esp
         pushad
         mov ecx, [ebp+8] ; Get first argument p
         call my_strlen
         mov ecx, [ebp+8] ; Get first argument
         mov ebx, [ebp+12] ; 2nd argument (outfile)
         mov edx, eax ; Count of bytes
         mov ebx, STDOUT
         mov eax, WRITE
         int 0x80 ; Linux system call
         popad
         mov esp, ebp
         pop ebp
         ret


encoder:
        push  dword ebp
        mov ebp, esp
        pushad
 loop:  mov eax, READ
        mov ebx, [infile]
        mov ecx, buff
        mov edx, 1
        int 0x80
        cmp eax ,0
        jz afterLoop
        mov eax, [buff]
        cmp eax, 0
        jz afterLoop
        mov edx, 'A'
        cmp eax,edx
        jb afterBoundCheck
        mov edx, 'z'
        cmp eax,edx
        ja afterBoundCheck
        inc eax
        mov [buff], eax
afterBoundCheck:
        mov eax, buff
        mov ebx, [outfile]
        push ebx
        push eax
        call my_puts
        add esp, 8
        jmp loop
afterLoop:
        push dword [outfile]
        push dword BACKSPACE
        call my_puts
        add esp, 8        
        popad
        mov esp, ebp
        pop ebp
        ret

closeFile:
        push ebp
        mov ebp, esp
        pushad
        mov eax, CLOSE
        mov ebx, [ebp+8]
        int 0x80
 