#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct virus {
    unsigned short SigSize; //2
    char virusName[16];     //16
    unsigned char* sig;     //8
} 
virus;

int check_endianness(FILE *file) {
    char magic[5];
    fread(magic, 1, 4, file);
    magic[4] = '\0';

    if (strcmp(magic, "VIRL") == 0) {
        // VIRL in little-endian
        return 1;
    } else if (strcmp(magic, "VIRB") == 0) {
        // VIRB in big-endian
        return 2;
    } else {
        // not recognized
        return 0;
    }
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
    free( virus ->sig);
    free(virus);
    return NULL;
  }

  return virus;
}