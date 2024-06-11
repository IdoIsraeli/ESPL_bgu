#include "LineParser.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h> // for errno


#define BUFFER_SIZE 2048

void print_cwd(){
    char path[PATH_MAX];
    getcwd(path,PATH_MAX);
    fprintf(stdout,"the current working directory is: %s\n",path);
}

void execute(cmdLine *pCmdLine){
    if(strcmp(pCmdLine->arguments[0],"quit")==0){exit(EXIT_SUCCESS);}
    int returnVal=execvp(pCmdLine->arguments[0],pCmdLine->arguments);
    if(returnVal<0){
      perror("Can't execute the command");
      exit(EXIT_FAILURE);
  }
}

int main(int argc, char const *argv[]) {
  FILE* input = stdin;
  char buf[BUFFER_SIZE];
  while(1){
    print_cwd();
    fgets(buf,BUFFER_SIZE,input);
    cmdLine* line = parseCmdLines(buf);
    execute(line);
    freeCmdLines(line);
  }
  return 0;
}
