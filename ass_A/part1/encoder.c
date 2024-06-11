#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]) 
{
  int debug_mode = 0;

  for (int i = 1; i < argc; i++) // starts from 1 to avoid argv[0] which is the name of the program exe.
  {
    if (debug_mode)
    {
      if(strcmp(argv[i], "-D") != 0)
        fprintf(stderr, "%s\n", argv[i]);
    }
    if (strcmp(argv[i], "+D") == 0) {
      debug_mode = 1; // turn debug mode on
    } else if (strcmp(argv[i], "-D") == 0) {
      debug_mode = 0; // turn debug mode off
    }

    
  }
  return 0;
}
