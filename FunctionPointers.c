//
// Created by dustyn on 3/23/24.
//

#include <stdio.h>
#include <stdlib.h>
//This function we will assign the address of to a pointer
int add(int a , int b)
{
    return a + b;
}
int functionpointers()
{
    //This pointer function declared here will then be ..
    int(*operation)(int,int);
    //Assigned the memory address of the add function above
    operation = &add;

    //We can see this in action here!
    printf("We will add 2 numbers using a pointer to a function ! 5 + 3 = %d",operation(5,3));
    return EXIT_SUCCESS;

}