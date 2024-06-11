#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize; //2
    char virusName[16];     //16
    unsigned char* sig;     //8
} virus;

typedef struct link link;
struct link {
    link *nextVirus;
    virus *vir;
};

struct newMenu{
    char *name;
    link* (*fun)(link*);
};

void PrintHex(FILE* output, unsigned char* buffer,unsigned short length) {
    for (int i=0; i<length; i++)
        fprintf(output,"%02hhX ", buffer[i]);
    fprintf(output,"\n\n");
}

virus* readVirus(FILE* input){
  virus* virus = malloc(sizeof(struct virus));
  if (!virus)
    return NULL;
  if(fread(virus,18,1,input)!=1)
  {
    free(virus);
    return NULL;
  }
  
  virus->sig=malloc(virus->SigSize);
  if(!virus -> sig)
  {
    free(virus);
    return NULL;
  }

  if(fread(virus->sig,virus->SigSize,1,input) != 1)
  {
    free(virus->sig);
    free(virus);
    return NULL;
  }
  return virus;
}

void printVirus(virus* virus, FILE* output){
  fprintf(output,"virus name: %s\n", virus->virusName);
  fprintf(output,"virus size: %d\n", virus->SigSize);
  fprintf(output,"signature:\n");
  PrintHex(output, virus->sig, virus->SigSize);
}

int getSize(FILE* file){
  fseek(file, 0L, SEEK_END);
  int fileSize = ftell(file);
  rewind(file);     //sets the file position to the beginning
  return fileSize;
}


/* Print the data of every link in list to the given stream. Each item followed by a newline character. */
void list_print(link *virus_list, FILE* input){
  link* currLink = virus_list;
  while (currLink!=NULL){
    printVirus(currLink->vir,input);
    currLink = currLink->nextVirus;
  }
}
 
link* newLink(virus* data){
    link* newLink = malloc(sizeof(struct link));
    newLink->vir=data;
    newLink->nextVirus=NULL;
    return newLink;
}

/*  Add a new link with the given data to the list 
    (either at the end or the beginning, depending on what your TA tells you),
    and return a pointer to the list (i.e., the first link in the list).
    If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data){
  if (virus_list == NULL)
    virus_list = newLink(data);
  else
    virus_list->nextVirus = list_append(virus_list->nextVirus,data);
  return virus_list;
}

/* Free the memory allocated by the list. */
void list_free(link *virus_list){
  link* currLink = virus_list;
  if(currLink!=NULL){
    list_free(currLink->nextVirus);
    free(currLink->vir->sig);
    free(currLink->vir);
    free(currLink);
  }
  return;
}

link* load_list(FILE* file){
    link* head=NULL;
    int fileSize = getSize(file);
    char buffer[4];
    fread(&buffer, 1, 4, file);
    int bytes = 4;       //readen bytes
    while(bytes<fileSize){
        virus* nextVirus = readVirus(file);  //to free??
        head=list_append(head,nextVirus);
        bytes=bytes+18+nextVirus->SigSize;
    }
    fclose(file);
    return head;
}

link* load_signatures(link* link){
  char* fileName=NULL;
  char buf[BUFSIZ];
  printf("Enter signature file name:\n");
  fgets(buf,sizeof(buf),stdin);
  sscanf(buf,"%ms",&fileName);
  FILE* file = fopen(fileName,"rb");
  free(fileName);
  if(file==NULL){
    fprintf(stderr,"Reading File Error\n");
    exit(EXIT_FAILURE);
  }
  return load_list(file);
}

link *print_signatures(link *list){
  list_print(list, stdout);
  return list;
}

void detectViruses()
{
  printf("Not implemented yet \n");
}

void fixFile()
{
  printf("Not implemented yet \n");
}

void quit()
{
  exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
  link* list = NULL;
  struct newMenu menu[] = {{"Load signatures",load_signatures},{"Print signatures",print_signatures},{"Detect viruses",detectViruses},{"fix file",fixFile},{"Quit",quit},{NULL, NULL}}; // need to add detect, fix, quit!!
  int bounds = sizeof(menu) / (sizeof(struct newMenu))-1;
  int option;
  while (1){
    printf("Please choose a function:\n");
    for (int i = 0; i < bounds; i++){
      printf("%d) %s\n", i + 1, menu[i].name);
    }
    printf("Option: ");
    scanf("%d", &option);
    fgetc(stdin);
    if (option < 1 || option > bounds){
      printf("Not within bounds\n");
      exit(EXIT_SUCCESS);
    }
    else{
      list = menu[option-1].fun(list);
      printf("\n");
    }
  }
  return 0;
}