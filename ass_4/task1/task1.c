#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <elf.h>

#define  NAME_LEN  128
#define  BUF_SZ    10000
#define  FILE_NAME_SZ   100

int debug = 0;
int displayToggle = 0;

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

void toggleDebugMode(state* s) {
  if(debug==0){
    printf("Debug flag now on\n");
    debug=1;
  }
  else{
    printf("Debug flag now off\n");
    debug=0;
  }
}

void setFileName(state* s) {
  printf("Enter new file name:\n");
  char newFileName[FILE_NAME_SZ];
  fscanf(stdin,"%s",newFileName);
  strcpy (s->file_name,newFileName);
  if(debug){
    fprintf(stderr,"Debug: file name set to %s\n",newFileName);
  }
}

int isUnitSizeValid(int n){
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
  if (!(isUnitSizeValid(newUnitSize))){
     printf("Invalid unit size\n");
     return;
  }
  s->unit_size = newUnitSize;
}

void loadIntoMemory(state* s) {
  if(strcmp(s->file_name,"")==0){
    printf("empty file name\n");
    return;
  }
  FILE* file = fopen(s-> file_name, "r+");
  if(file == NULL){
    printf("Error open file\n");
    return;
  }
  fprintf(stdout, "Please enter <location> <length>:\n");
  int location=0;
  int length=0;
  fscanf(stdin,"%x %d",&location,&length);
  if(debug){
    fprintf(stderr,"file name: %s\n",s-> file_name);
    fprintf(stderr,"location: %d\n",location);
    fprintf(stderr,"length: %d\n",length);
  }
  fseek (file, location, SEEK_SET);
  s->mem_count = length*s->unit_size;
  fread(s->mem_buf,s->unit_size,length,file);
  fclose(file);
  fprintf(stdout,"Loaded %d units into memory\n",length);
}

char* unit_to_format_hex(int unit_size ) {
    static char* formats_hex[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
    return formats_hex[unit_size-1];
}
char* unit_to_format_dec(int unit_size ) {
    static char* formats_decimal[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
    return formats_decimal[unit_size-1];
}

/* Prints the buffer to screen by converting it to text with printf */
void print_units(FILE* output, void* buffer, int count , int unit_size, int printHexa) {
    void* end = buffer + unit_size*count;
    while (buffer < end) {
        //print ints
        int var = *((int*)(buffer));
        if(printHexa){fprintf(output,unit_to_format_hex(unit_size), var);}
        else{fprintf(output,unit_to_format_dec(unit_size), var);}
        buffer += unit_size;
    }
}

void memoryDisplay(state* s){
  fprintf(stdout, "Enter address and length:\n");
  int u = 0;
  int addr = 0;
  fscanf(stdin,"%x %d",&addr,&u);
  if(debug)
      printf("selected address: %x\nselected length: %d\n",addr,u);
  if(addr==0)
    {print_units(stdout,&(s->mem_buf),u,s->unit_size,displayToggle);}
  else
    {print_units(stdout,&addr,u,s->unit_size,displayToggle);}
}

void saveIntoFile(state* s){
  fprintf(stdout,"Please enter <source-address> <target-location> <length>:\n");
  int source_address = 0;
  int target_location = 0;
  int length = 0;
  fscanf(stdin,"%x %x %d",&source_address,&target_location,&length); 
  FILE* file = fopen(s->file_name,"r+");
  if(file==NULL) {
    printf("Error open file\n");
    return;
  }
  fseek(file,0,SEEK_END);
  if(target_location > ftell(file)){ 
    perror("target location is greater then file size");
    return;
  }
  fseek(file,0,SEEK_SET); //back to start of file
  fseek(file,target_location,SEEK_SET);
  if(source_address==0){fwrite(&(s->mem_buf),s->unit_size,length,file);}
  else{fwrite(&(source_address),s->unit_size,length,file);}
  fclose(file);
}

void memoryModify(state* s) {
  fprintf(stdout,"Please enter <location> <val>:\n");
  int location=0;
  int val=0;
  fscanf(stdin,"%x %x",&location,&val);
  if(debug){
      printf("selected location: %x\nselected value: %x\n",location,val);
  }
  memcpy(&s->mem_buf[location],&val,s->unit_size);
}

void quit(state* s) {
    if(debug){fprintf(stderr,"quitting\n");}
    exit(0);
}

void display (fun_desc menu[] , state* s){
    if(debug){
      fprintf(stderr,"unit_size: %d\n",s->unit_size);
      fprintf(stderr,"file_name: %s\n",s->file_name);
      fprintf(stderr,"mem_count: %d\n\n",s->mem_count);
    }
    fprintf(stdout, "choose action:\n");
    int i=0 ;
    while(menu[i].name != NULL) {
      fprintf(stdout,"%d) %s\n",i,menu[i].name);
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

void toggleDisplayMode(state* s) {
  if(displayToggle == 0){
    printf("display flag now on, hexadecimal representation\n");
    displayToggle=1;
  }
  else{
    printf("display flag now off, decimal representation\n");
    displayToggle=0;
  }
}

/*void memoryDisplay(state* s) {
    fprintf(stdout,"not implemented yet\n");
}

void saveIntoFile(state* s) {
    fprintf(stdout,"not implemented yet\n");
}

void memoryModify(state* s) {
    fprintf(stdout,"not implemented yet\n");
}
*/

int main(int argc, char **argv){
  state* s = calloc (1 , sizeof(state));

  fun_desc menu[] = { { "Toggle Debug Mode", toggleDebugMode }, { "Set File Name", setFileName }, { "Set Unit Size", setUnitSize },
                      { "Load into memory", loadIntoMemory }, { "Toggle display mode", toggleDisplayMode }, { "Memory display", memoryDisplay },
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