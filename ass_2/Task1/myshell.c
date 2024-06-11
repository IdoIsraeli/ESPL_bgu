#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> // for errno
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 2048

void print_cwd(){
    char path[PATH_MAX];
    getcwd(path,PATH_MAX);
    fprintf(stdout,"the current working directory is: %s\n",path);
}



void execute(cmdLine *pCmdLine,int debug){
    if(strcmp(pCmdLine->arguments[0],"quit")==0){
        freeCmdLines(pCmdLine);
         exit(EXIT_SUCCESS);
        }
    if(strcmp(pCmdLine->arguments[0],"cd")==0){
        if(chdir(pCmdLine->arguments[1])<0){
          perror("cant execute cd command");
        }
    }
    else{
    int pid; // of the child
    if(!(pid = fork())){
       
        if(execvp(pCmdLine->arguments[0],pCmdLine->arguments)<0){
        perror("Can't execute the command");
        exit(EXIT_FAILURE);
        }
    }
    //printf("debug: %d ",pid);
    if(debug)
      fprintf(stderr, "PID: %d\nExecuting command: %s\n",pid,pCmdLine->arguments[0]);
    if(pCmdLine->blocking)  //& , ask TA if should i check if this char is the last char of any argv[i] ?
      waitpid(pid,NULL,0);
    }
}

int main(int argc, char const *argv[]){
  FILE* input = stdin;
  char buf[BUFFER_SIZE];
  int debug = 0;
  for(int i=1;i<argc;i++){
    if((strcmp("-D",argv[i])==0)){
      debug=1;
    }
  }
  while(1){
    print_cwd();
    fgets(buf,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buf);
    execute(line,debug);
    freeCmdLines(line);
  }
  return 0;
}
