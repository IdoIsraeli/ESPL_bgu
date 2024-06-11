#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
  char sign;  
  char *key;
  int keyIndex = 0;
  FILE *infile = stdin;
  FILE *outfile = stdout;

  // Loop over command-line arguments to find encoding key
  for (int i = 0; i < argc; i++) 
  {
    if (strncmp(argv[i], "+e",2) == 0) 
    {
      key = argv[i] + 2;
      sign = '+';
    } else if (strncmp(argv[i], "-e",2) == 0) 
    {
     key = argv[i] + 2;
     sign ='-';
    }
    else if (strncmp(argv[i], "-i",2) == 0) 
    {
     infile = fopen(argv[i]+2,"r");
    }
    else if (strncmp(argv[i], "-o",2) == 0) 
    {
    outfile = fopen(argv[i]+2,"w");
    }

  }
  // Loop over input characters
  int c = fgetc(infile);
  while (c != EOF) {
    // Apply encoding only to alpha-numeric characters
    if (isalnum(c)) 
    {
      if(key[keyIndex] == '\0')
        keyIndex = 0;
      if (isupper(c)) 
      {
        if(sign == '+')
          c = ((c + key[keyIndex]-48 -65) % 26) + 65; // wrap around from Z to A
        else 
          c = ((c - key[keyIndex]+48 -65 +26) % 26) + 65; // wrap around from Z to A

      } else if (islower(c)) 
      {
        if(sign == '+')
          c = ((c + key[keyIndex]-48 -97) % 26) + 97; // wrap around from z to a
        else
          c = ((c - key[keyIndex]+48 -97 +26) % 26) + 97; // wrap around from z to a
      } else 
      {
        if(sign == '+')
          c = ((c + key[keyIndex]-48 -48) % 10) + 48; // wrap around from 9 to 0
        else
          c = ((c - key[keyIndex]+48 -48 +10) % 10) + 48; // wrap around from 9 to 0
      }
    }
    keyIndex++;
    fputc(c, outfile);
    c = fgetc(infile);
  }

  // Close input and output streams
  fclose(infile);
  fclose(outfile);

  return 0;
}
