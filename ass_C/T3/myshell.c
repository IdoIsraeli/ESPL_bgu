#include "LineParser.h" 
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 2048

#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0

#define STDIN 0
#define STDOUT 1

#define HISTLEN 20

typedef struct process{
    cmdLine* cmd;                   /* the parsed command line*/
    pid_t pid; 		                /* the process id that is running the command*/
    int status;                     /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	        /* next process in chain */
} process;

/*global variables*/
process* globalProcessList;
char* historyArray[HISTLEN];
int newest_index = 0;
int oldest_index = 0;
int history_index = 0;


/*function declarations: */
void printSingleProcess(process*);
void execute(cmdLine* cmd,int);
void pipeLine(cmdLine* cmd);
void updateProcessList(process **);
void updateProcessStatus(process*,int,int);
int delete_terminated(process **);
void delete_single_process(process*);
char* get_nth_command(int);


void print_cwd()
{
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    fprintf(stdout, "the current working directory is: %s\n", path);
}

process* createNewProcess(cmdLine* cmd, pid_t pid){
    process* new_process = malloc(sizeof(struct process));
    new_process->cmd=cmd;
    new_process->pid=pid;
    new_process->status=RUNNING;
    new_process->next=NULL;
    return new_process;
}

/*creates a new proccess link ,and appends the list to it
(new link will be first in the list)*/
void addProcess(process** process_list, cmdLine* cmd, pid_t pid) {
    process* new_process = createNewProcess(cmd, pid);
    new_process->next = *process_list;
    *process_list = new_process;
}

char* getStatusAsString(int status){
    if(status == TERMINATED)
      return "Terminated";
    else if(status == RUNNING)
      return "Running";
    else          /*if(status == SUSPENDED)*/
      return "Suspended";
}

void printProcessList(process** process_list){
    updateProcessList(process_list);
    printf("PID\t\tCommand\t\tSTATUS\n");
    process* current_process = *process_list;
    while (current_process != NULL) {
        printSingleProcess(current_process);
        current_process = current_process->next;
    }
    while(delete_terminated(process_list)){}
}

void printSingleProcess(process* process){
    char command[100]="";
    if(process!=NULL && process->cmd->argCount>0)
        for(int i=0;i<process->cmd->argCount;i++){
        const char* argument = process->cmd->arguments[i];
        strcat(command,argument);
        strcat(command," ");
    }
    printf("%d\t\t%s\t%s\n",process->pid,command,getStatusAsString(process->status));
}

void free_list(process* process_list){
    process* curr_process=process_list;
    if(curr_process!=NULL){
        free_list(curr_process->next);
        //freeCmdLines(curr_process->cmd);
        delete_single_process(curr_process);
        //free(curr_process->cmd);
        //free(curr_process);
    }
}

void freeProcessList(process* process_list){
  free_list(process_list);
}

void updateProcessList(process **process_list){
    process* curr_process = (*process_list);
    while(curr_process!=NULL){
        int status;
        int wait = waitpid(curr_process->pid,&status,WNOHANG | WUNTRACED | WCONTINUED);
        if(wait!=0){    //status changed
            updateProcessStatus(curr_process,curr_process->pid,status);
        }
        curr_process=curr_process->next;
    }
}

void updateProcessStatus(process* process_list, int pid, int status){
  int curr_status=RUNNING;
  if(WIFEXITED(status) || WIFSIGNALED(status))
    curr_status=TERMINATED;
  else if(WIFSTOPPED(status))
    curr_status=SUSPENDED;
  else if(WIFCONTINUED(status))
    curr_status=RUNNING;
  process_list->status=curr_status;
}

void delete_single_process(process* process){
    freeCmdLines(process->cmd);
    process->cmd=NULL;
    process->next=NULL;
    free(process);
    process=NULL;
}

int delete_terminated(process** process_list){
    process* curr_process = *process_list;
    process* prev_process;
    /*deleting the head*/
    if(curr_process!=NULL && curr_process->status==TERMINATED){
        *process_list=curr_process->next;
        delete_single_process(curr_process);
        return 1;
    }
    /*get the next terminated process*/
    while (curr_process!=NULL && curr_process->status!=TERMINATED){
        prev_process=curr_process;
        curr_process=curr_process->next;
    }
    /*we didn't delete*/
    if(curr_process==NULL)
      return 0;
    else{
        prev_process->next=curr_process->next;
        delete_single_process(curr_process);
        return 1;
    }
}


int special_commands(cmdLine *command)
{
    int n;
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
            perror("cant execute cd command");
    }
    else if (strcmp(command->arguments[0], "procs") == 0){
        printProcessList(&globalProcessList);
        is_special =1;
    }
    else if (strcmp(command->arguments[0], "wake") == 0){ // needs to be suspend
        kill(atoi(command->arguments[1]),SIGCONT);
        is_special =1;
    }
    else if (strcmp(command->arguments[0], "kill") == 0){
        kill(atoi(command->arguments[1]),SIGINT);
        is_special =1;
    }
    else if (strcmp(command->arguments[0], "suspend") == 0){
        kill(atoi(command->arguments[1]),SIGTSTP);
        is_special =1;
    }
    else if(strcmp(command->arguments[0],"history")==0){
        int start_index, end_index, count =1;
        start_index = (history_index >= HISTLEN) ? (newest_index % HISTLEN) : 0;
        end_index = (oldest_index != 0) ? (HISTLEN) : newest_index;
        //printf("the start index is %d\nthe end index is %d\nthe oldest index is%d\n",start_index,end_index,oldest_index);
        for (int i = 0; i < end_index; i++) {
        printf("%d: %s\n", count++, historyArray[(start_index++)%HISTLEN]);
        }
        is_special = 1;
    }
    else if(strcmp(command->arguments[0],"!!")==0){
        if(history_index != 0){
            printf("%s",historyArray[newest_index-1]);
            cmdLine *line = parseCmdLines(historyArray[newest_index-1]);
            execute(line,0);
        }
        is_special =1;
    }
    else if (strncmp(command->arguments[0], "!", 1) == 0 && sscanf((command->arguments[0])+1, "%d",&n) == 1){ // the input is of format "!n"
        char* unparsedLine = get_nth_command(n);
        //printf("ido, unparedline is:%s\n",unparsedLine);
        if(unparsedLine == NULL){
            printf("error, invalid number for coammand of this type!\n");
            return 1;
        }
        cmdLine *line = parseCmdLines(unparsedLine);
        execute(line,0);
        is_special = 1;
    }
    return is_special;
}

void addToHistory(char * input){
    int n;
    //printf("this current input is:%s\n",input);
  if(strcmp(input,"!!\n") != 0 && (strncmp(input, "!", 1) != 0 || sscanf(input+1, "%d",&n) != 1)){
  char* copy =malloc(strlen(input)+1);
  strcpy(copy,input);
  if (history_index >= HISTLEN) {
    free(historyArray[oldest_index]);
    oldest_index = (oldest_index + 1) % HISTLEN;
  } else {
    history_index++;
  }
  historyArray[newest_index] = copy;
  newest_index =(newest_index +1)% HISTLEN;
}
}

char* get_nth_command(int n){
    if(n<0 || n>HISTLEN || n>history_index)
    return NULL;
    //int index = (newest_index - n + HISTLEN + 1) % HISTLEN;
    char* command = historyArray[n];
    return command;
}

void execute(cmdLine * cmd,int debug){
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
            //printf("im in input redirect\n");
            close(STDIN);
            if(!fopen(cmd->inputRedirect, "r")){
                perror(strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        if (cmd->outputRedirect != NULL){
           // printf("im in output redirect\n");
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
    addProcess(&globalProcessList,cmd,pid);
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
        if (cmd->inputRedirect != NULL){
            close(STDIN);
            if(!fopen(cmd->inputRedirect, "r")){
                perror(strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
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
         if (cmd->next->outputRedirect != NULL){
            printf("im in output redirect\n");
            close(STDOUT);
            if(!fopen(cmd->next->outputRedirect, "w")){
                perror(strerror(errno));
                exit(EXIT_FAILURE); 
            }
        }
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
        cmdLine * temp = cmd->next;
        cmd->next = NULL;
        addProcess(&globalProcessList,cmd,c1_pid);
        addProcess(&globalProcessList,temp,c2_pid);
    }
}


int main(int argc, char const *argv[]){
    FILE *input = stdin;
    char buf[BUFFER_SIZE];
    for (int i=0;i<HISTLEN; i++){
    historyArray[i]=0;
    }
    int debug = 0;
    globalProcessList = NULL;
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
        if(!fgets(buf, BUFFER_SIZE, input)){
            if(globalProcessList != NULL){
                freeProcessList(globalProcessList);
            }
            return 0;
        }
        addToHistory(buf);
        cmdLine *line = parseCmdLines(buf);
        if(!line){
            continue;
        }
        execute(line,debug);
    }
    return 0;
}

