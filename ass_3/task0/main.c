#include "util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291

extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
  int i;
  for (i = 0; i < argc; i++) {
    char* arg = argv[i];
    int len = 0;
    while (arg[len]) {
      len++;
    }
    system_call(4, 1, arg, len);
    system_call(4, 1, "\n", 1);
    }

  return 0;
}