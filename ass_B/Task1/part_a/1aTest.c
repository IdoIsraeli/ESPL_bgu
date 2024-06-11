#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize; //2
    char virusName[16];     //16
    unsigned char* sig;     //8
} virus;

int check_magic(FILE *file) {
    char magic[5];
    fread(magic, 1, 4, file);
    magic[4] = '\0';

    if (strcmp(magic, "VIRL") == 0 || strcmp(magic, "VISL") == 0 ) 
    {
        return 1;
    } else if (strcmp(magic, "VIRB") == 0 || strcmp(magic, "VISB") == 0) 
    {
        return 2;
    } else 
    {
        return 0;  // not recognized
    }
}

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


int main(int argc, char **argv) {
  FILE* input = fopen(argv[1],"rb");
  if (input==NULL){
    fprintf(stderr,"Reading File Error\n");
    exit(EXIT_FAILURE);
  }
  if(check_magic(input) == 0)
  {
    fprintf(stderr,"magic number Error\n");
    exit(EXIT_FAILURE);
  } 
  FILE* output = stdout;
  int fileSize = getSize(input);
  char buffer[4];
  fread(&buffer, 1, 4, input);
  int bytes = 4;   //bytes that were read already
  while(bytes<fileSize){
    virus* nextVirus = readVirus(input);
    printVirus(nextVirus,output);
    bytes = bytes+18+nextVirus->SigSize;
    free(nextVirus);
  }
  return 0;
}