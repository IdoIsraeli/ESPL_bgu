#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//-------------------Task 2 stuff------------------------

char* map(char *array, int array_length, char (*f) (char))
{
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  /* TODO: Complete during task 2.a */
  for(int i=0; i<array_length; i++)
    {
        mapped_array[i] = (*f)(array[i]);
    }
  return mapped_array;
}

char my_get(char c)
{
  return fgetc(stdin);
}

char cprt(char c) 
{
  if (c >= 0x20 && c <= 0x7E) 
    printf("%c\n", c);
  else 
    printf(".\n");
  return c;
}

char encrypt(char c) 
{
  if (c >= 0x20 && c <= 0x7E)
    return c + 1;
  else 
    return c;
}

char decrypt(char c) 
{
  if (c >= 0x20 && c <= 0x7E) 
    return c - 1;
  else 
    return c;
}

char xprt(char c) 
{
    if (c >= 0x20 && c <= 0x7E) 
    printf("%x\n", c);
  else 
    printf(".\n");
  return c;
}

int main() 
{
    char carray[5] = "";
    char* temp = carray;
    char input[10];
    int opt;
    int i;
    struct fun_desc 
        {
        char *name;
        char (*fun)(char);
        };

    struct fun_desc menu[] = { {"get string", my_get}, {"print string", cprt}, {"print hexadecimal",xprt}, {"encrypt",encrypt}, {"decrypt",decrypt}, { NULL, NULL } };

    printf("Select operation from the following menu:\n");
    for(i=0; i < (sizeof(menu) / sizeof(menu[0]))-1 ;i++)
        printf("%d) %s\n",i, menu[i].name);
    fgets(input, sizeof(input), stdin);
    sscanf(input,"%d",&opt);

    while (1) 
    {   // repeat forever
    if(opt >=0 && opt<=4)
    {
        printf("within bounds\n");
        temp = map(temp,5,menu[opt].fun);

    }
    else
    {
        printf("not within bounds\n");
        break;
    }    

        printf("Select operation from the following menu:\n");
        for(int i=0;i < (sizeof(menu) / sizeof(menu[0]))-1;i++)
        {
            printf("%d) %s\n",i,menu[i].name);
        }
        if (fgets(input, sizeof(input), stdin) == NULL)   // check for EOF
            break;   // exit if EOF
        
        sscanf(input,"%d",&opt);
    
    }
    return 0;
}



