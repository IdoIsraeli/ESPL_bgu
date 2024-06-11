#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define MSG_SIZE 6 // size of message

int main(void) {
    int fd[2]; // file descriptors
    pid_t pid; 
    char msg[MSG_SIZE] = "hello";
    char buf[MSG_SIZE];
    
    if (pipe(fd) == -1) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    
   
   if(!(pid = fork())){ // child process
    close(fd[0]);
    write(fd[1], msg ,MSG_SIZE);
   }
   else{ //parent process
    close(fd[1]);
    read(fd[0],buf,MSG_SIZE);
    printf("%s\n",buf); 
   }
   return 0;
}
