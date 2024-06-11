#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> // for errno
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define STDIN 0
#define STDOUT 1
#define BUFFER_SIZE 2048

int debug;
void execute(cmdLine* cmd);
void pipeLine(cmdLine* cmd);


void print_cwd()
{
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    fprintf(stdout, "the current working directory is: %s\n", path);
}

int special_commands(cmdLine *command)
{
    int is_special = 0;
    if (strcmp(command->arguments[0], "quit") == 0)
    {
        is_special = 1;
        freeCmdLines(command);
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(command->arguments[0], "cd") == 0)
    {
        is_special = 1;
        if (chdir(command->arguments[1]) < 0)
        {
            perror("cant execute cd command");
        }
    }
    else if (strcmp(command->arguments[0], "suspend") == 0){
        kill(atoi(command->arguments[1]),SIGTSTP);
        is_special =1;
    }
    else if (strcmp(command->arguments[0], "wake") == 0){
        kill(atoi(command->arguments[1]),SIGCONT);
        is_special =1;
    }
    else if (strcmp(command->arguments[0], "kill") == 0){
        kill(atoi(command->arguments[1]),SIGINT);
        is_special =1;
    }
    return is_special;
}

void execute(cmdLine * cmd){
    if(!special_commands(cmd)){
    int pid; // of the child
    if(cmd->next !=NULL){
        if(cmd -> outputRedirect){
            return;
        }
        if(cmd ->next ->inputRedirect){
            return;
        }
            pipeLine(cmd);
        }
    else{
    if (!(pid = fork())){
        if (cmd->inputRedirect != NULL){
            printf("im in input redirect\n");
            close(STDIN);
            if(!fopen(cmd->inputRedirect, "r")){
                perror(strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        if (cmd->outputRedirect != NULL){
            printf("im in output redirect\n");
            close(STDOUT);
            if(!fopen(cmd->outputRedirect, "w")){
                perror(strerror(errno));
                exit(EXIT_FAILURE); 
            }
        }
        if(execvp(cmd->arguments[0], cmd->arguments)<0){
        perror("Can't execute the command");
        exit(EXIT_FAILURE);
        }
    }
    }
    if(debug)
        fprintf(stderr, "PID: %d\nExecuting command: %s\n", pid, cmd->arguments[0]);

    if(cmd->blocking)
    {
        waitpid(pid, NULL, 0);
    }
  }
}

void pipeLine(cmdLine* cmd){
    pid_t c1_pid, c2_pid;
    int fd[2]; //fileDescriptors

    if (pipe(fd)==-1){//created a pipe
      perror(strerror(errno));
      _exit(EXIT_FAILURE);
    }
    c1_pid = fork(); // fork a first child process
    if(c1_pid == -1){
        perror(strerror(errno));
        _exit(EXIT_FAILURE);
    }
    if(!(c1_pid)){// in the first child process
        close(STDOUT);
        if(dup(fd[1]) == -1){
            printf("dup2 error in child 1");
        }
        close(fd[1]);// close duplicated FD
        close(fd[0]);
        execvp(cmd ->arguments[0],cmd ->arguments);// execute cmd in child 1
        perror(strerror(errno));//avoid fork bomb
        _exit(EXIT_FAILURE);
    }
    else{
        close(fd[1]);// closing write end of the pipe in the parent process
        c2_pid = fork(); // fork a second child process
        if (c2_pid==-1){
            perror(strerror(errno));
            _exit(EXIT_FAILURE);
        }
        if(!c2_pid){//in the second child
        close(STDIN);
        if(dup(fd[0]) == -1){
            printf("dup2 error in child 2");
        }
        close(fd[0]);
        close(fd[1]);
        execvp(cmd->next->arguments[0],cmd->next->arguments);//execute cmd in child 2
        perror(strerror(errno));//avoid fork bomb
        _exit(EXIT_FAILURE);
        }
        else{
            close(fd[0]);//close read end of the pipe in the parent process
            waitpid(c1_pid,NULL,0);
            waitpid(c2_pid,NULL,0);
        }
    }
}

int main(int argc, char const *argv[]){
    FILE *input = stdin;
    char buf[BUFFER_SIZE];
    debug = 0;
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp("-d", argv[i]) == 0))
        {
            debug = 1;
        }
    }
    while (1)
    {
        print_cwd();
        fgets(buf, BUFFER_SIZE, input);
        cmdLine *line = parseCmdLines(buf);
        if(!line){
            continue;
        }
        execute(line);
        freeCmdLines(line);
    }
    return 0;
}
