#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define  NAME_LEN  128
#define  BUF_SZ    10000
#define  FILE_NAME_SZ    100
#define STDIN 0

int debug = 0;

typedef struct {
  char debug_mode;
  char file_name[NAME_LEN];
  int unit_size;
  unsigned char mem_buf[BUF_SZ];
  size_t mem_count;
} state;

typedef struct {
  char *name;
  void (*fun)(state*);
}fun_desc;

void toggleDebugMode(state* s){
  if(debug==0){
    printf("Debug flag now on\n");
    debug=1;
  }
  else{
    printf("Debug flag now off\n");
    debug=0;
  }
}

void setFileName(state* s){
  printf("Enter new File Name: \n");
  char newFileName[FILE_NAME_SZ];
  fscanf(stdin,"%s",newFileName);
  strcpy(s->file_name,newFileName);
  if(debug){
    printf("Debug: file name set to %s\n", newFileName);
  }
}

int isValid(int n){
  if(n==1 || n==2 || n==4){
     return 1;
  }
  return 0;
}

void setUnitSize(state* s) {
  fprintf(stdout,"Enter new unit size:\n");
  int newUnitSize = 0;
  fscanf(stdin,"%d",&newUnitSize);
  if(debug){
    fprintf(stderr,"Debug: unit size set to %d\n",newUnitSize);
  }
  if (!(isValid(newUnitSize))){
     printf("Invalid unit size\n");
     return;
  }
  s->unit_size = newUnitSize;
}

void quit (state* s) {
    if (debug) { printf("quitting\n");}
    exit(0);
}

void printDebugBeforeMenu (state* s){
    printf("unit_size : %d\n", s->unit_size );
    printf("file_name : %s\n", s->file_name );
    printf("mem_count : %d\n\n", s->mem_count );
}

void display (fun_desc menu[] , state* s){
    if (debug){
      printDebugBeforeMenu(s);
    }

    fprintf(stdout, "Please choose a function:\n");

    int i=0 ;
    while(menu[i].name != NULL) {
      fprintf(stdout, "%d) %s\n", i, menu[i].name);
      i++;
    }

    fprintf(stdout, "Option: ");
}

int getUserInput (int bounds){
  int option;
  scanf("%d", &option);
  if (option >= 0 && option < bounds){
    fprintf(stdout, "\n" );
    return option;
  }
  else{
    fprintf(stdout, "Not within bounds\n\n" );
    return -1;
  }
}

void loadIntoMemory(state* s) {
    fprintf(stdout,"not implemented yet\n");
}

void ToggleDisplayMode(state* s) {
    fprintf(stdout,"not implemented yet\n");
}

void memoryDisplay(state* s) {
    fprintf(stdout,"not implemented yet\n");
}

void saveIntoFile(state* s) {
    fprintf(stdout,"not implemented yet\n");
}

void memoryModify(state* s) {
    fprintf(stdout,"not implemented yet\n");
}


int main(int argc, char **argv){
  state* s = calloc (1 , sizeof(state));

  fun_desc menu[] = { { "Toggle Debug Mode", toggleDebugMode }, { "Set File Name", setFileName }, { "Set Unit Size", setUnitSize },
                      { "Load into memory", loadIntoMemory }, { "Toggle display mode", ToggleDisplayMode }, { "Memory display", memoryDisplay },
                      { "Save into file", saveIntoFile }, { "Memory modify", memoryModify }, { "Quit", quit } ,{ NULL, NULL } };

  size_t bounds=0;
  while ( menu[bounds].name != NULL){
    bounds = bounds +1;
  }

  while (1) {

    display(menu , s);
    int option = getUserInput (bounds);
    if (option != -1) { menu[option].fun(s); }

    printf("\n");

  }


}