#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5; //uninitialized var, strored in bss
int addr6; //uninitialized var, strored in bss

int foo()
{
    return -1;
}
void point_at(void *p);
void foo1();
char g = 'g';
void foo2();

int main(int argc, char **argv)
{ 
    int addr2;
    int addr3;
    char *yos = "ree";
    int *addr4 = (int *)(malloc(50));
    
    printf("Print addresses:\n");
    printf("- &addr2: %p\n", &addr2);
    printf("- &addr3: %p\n", &addr3);
    printf("- foo: %p\n", &foo);
    printf("- &addr5: %p\n", &addr5);

    printf("- argc %p\n", &argc);
    printf("- argv %p\n", argv);
    printf("- &argv %p\n", &argv);
    
    printf("Print distances:\n");
    point_at(&addr5);

    printf("Print more addresses:\n");
    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4);
    printf("- &addr4: %p\n", &addr4);

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);
    
    printf("Print another distance:\n");
    printf("- &foo2 - &foo1: %ld\n", (long) (&foo2 - &foo1));

   
    printf("Arrays Mem Layout (T1b):\n");
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3]; 
    /* task 1 b here */

    printf("hexadecimal values of the arrays: \n");
    printf("value of iarray: %p\n", iarray);
    printf("value of iarray: %p\n", iarray+1);
    printf("value of farray: %p\n", farray);
    printf("value of farray: %p\n", farray+1);
    printf("value of darray: %p\n", darray);
    printf("value of darray: %p\n", darray+1);
    printf("value of carray: %p\n", carray);
    printf("value of carray: %p\n", carray+1);
    
    
    
    printf("Pointers and arrays (T1d): ");
    int iarray2[] = {1,2,3};
    char carray2[] = {'a','b','c'};
    int* iarray2Ptr;
    char* carray2Ptr; 
    /* task 1 d here */
    
    printf("Command line arg addresses (T1e):\n");
    /* task 1 e here */
    
    return 0;
}

void point_at(void *p)
{
    int local;
    static int addr0 = 2;
    static int addr1;

    long dist1 = (size_t)&addr6 - (size_t)p;
    long dist2 = (size_t)&local - (size_t)p;
    long dist3 = (size_t)&foo - (size_t)p;

    printf("- dist1: (size_t)&addr6 - (size_t)p: %ld\n", dist1);
    printf("- dist2: (size_t)&local - (size_t)p: %ld\n", dist2);
    printf("- dist3: (size_t)&foo - (size_t)p:  %ld\n", dist3);
    
    printf("Check long type mem size (T1a):\n");
    /* part of task 1 a here */

    printf("- addr0: %p\n", &addr0);
    printf("- addr1: %p\n", &addr1);
}

void foo1()
{
    printf("foo1\n");
}

void foo2()
{
    printf("foo2\n");
}