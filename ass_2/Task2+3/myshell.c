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

#define BUFFER_SIZE 2048

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

void execute(cmdLine * pCmdLine, int debug){
    if(!special_commands(pCmdLine)){
    int pid; // of the child
    if (!(pid = fork()))
    {
        if (pCmdLine->inputRedirect != NULL)
        {
            close(STDIN_FILENO);
            fopen(pCmdLine->inputRedirect, "r");
        }
        if (pCmdLine->outputRedirect != NULL)
        {
            close(STDOUT_FILENO);
            fopen(pCmdLine->outputRedirect, "w");
        }
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("Can't execute the command");
        exit(EXIT_FAILURE);
    }
    if (debug)
    {
        fprintf(stderr, "PID: %d\nExecuting command: %s\n", pid, pCmdLine->arguments[0]);
    }
    if (pCmdLine->blocking)
    {
        int i;
        waitpid(pid, &i, 0);
    }
  }
}

int main(int argc, char const *argv[])
{
    FILE *input = stdin;
    char buf[BUFFER_SIZE];
    int debug = 0;
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
        execute(line, debug);
        freeCmdLines(line);
    }
    return 0;
}
