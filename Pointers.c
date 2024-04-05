//
// Created by dustyn on 3/21/24.
//
#include <stdio.h>
#include <stdlib.h>

int pointers(void)
{
    //this allocates a memory address to store the integer 10
    int x = 10;

    //however if you try to, as you would in a higher language, assign the value of x to the another, you are just duplicating the value in memory
    //You would want to address the specific memory address in which the value of x is located, you do this with address-of operator &x;
    int *pointer = &x; // = x;


    //You can utilize pointers to other pointers as well which can be useful in some situations
    int **point2point = &pointer;
    //We can dereference the pointer to a pointer with the ** notation which will get the actual value stored at the address by following the pointers to the address
    **point2point = 20;

    //Now x is changed because we dereferenced the pointer to the other pointer which then changed the initial value x at it's memory address
    printf("%d\n",x);
    return EXIT_SUCCESS;


}