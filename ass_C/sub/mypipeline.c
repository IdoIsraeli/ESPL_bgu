#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define STDIN 0
#define STDOUT 1

int main(int argc, char** argv){
    pid_t c1_pid, c2_pid;

    // maybe need to check for "-d" and debug

    char* const ls[3] = {"ls", "-l", 0};
    char* const tail[4] = {"tail", "-n", "2", 0};
    int fd[2]; //fileDescriptors

    if (pipe(fd)==-1){//created a pipe
      perror(strerror(errno));
      _exit(EXIT_FAILURE);
    }
    fprintf(stderr,"parent_process>forking...\n");
    c1_pid = fork(); // fork a first child process
    if(c1_pid == -1){
        perror(strerror(errno));
        _exit(EXIT_FAILURE);
    }
    fprintf(stderr,"parent_process>created process with id: %d \n",getpid());
    if(!(c1_pid)){// in the first child process
        fprintf(stderr,"child1_process>redirecting stdout to the write end of the pipe\n");
        close(STDOUT);
        dup2(fd[1],STDOUT);
        close(fd[1]);// close duplicated FD
        fprintf(stderr,"child1_process>going to execute cmd...\n");
        execvp(ls[0], ls);// execute cmd in child 1
        perror(strerror(errno));//avoid fork bomb
        _exit(EXIT_FAILURE);
    }
    else{
        fprintf(stderr,"parent_process>closing the write end of the pipe...\n");
        close(fd[1]);// closing write end of the pipe in the parent process
        c2_pid = fork(); // fork a second child process
        if (c2_pid==-1){
            perror(strerror(errno));
            _exit(EXIT_FAILURE);
        }
        if(!c2_pid){//in the second child
        fprintf(stderr,"child2_process>redirecting stdin to the read end of the pipe\n");
        close(STDIN);
        dup2(fd[0],STDIN);
        close(fd[0]);
        fprintf(stderr,"child2_process>going to execute cmd...\n");
        execvp(tail[0],tail);//execute cmd in child 2
        perror(strerror(errno));//avoid fork bomb
        _exit(EXIT_FAILURE);
        }
        else{
            fprintf(stderr,"parent_process>closing the read end of the pipe...\n");
            close(fd[0]);//close read end of the pipe in the parent process
            fprintf(stderr,"parent_process>waiting for child processes to be terminated...\n");
            waitpid(c1_pid,NULL,0);
            waitpid(c2_pid,NULL,0);
        }

    }
    fprintf(stderr,"parent_process>exiting...\n");
    return 0;
}
