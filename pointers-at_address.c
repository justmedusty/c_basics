//
// Created by dustyn on 3/21/24.
//
#include <stdio.h>
int notmain2(void)
{
    //this allocates a memory address to store the integer 10
    int x = 10;

    //however if you try to, as you would in a higher language, assign the value of x to the another, you are just duplicating the value in memory
    //You would want to address the specific memory address in which the value of x is located, you do this with address-of operator &x;
    int *pointer = &x; // = x;


    //You can utilize pointers to other pointers as well which can be useful in some situations
    int **point2point = &pointer;
    **point2point = 20;

    //notice when we set our 2nd pointer to 20, since it points to the address of pointer which points ot the address of x, this changes the value of x.
    printf("%d\n",x);


}