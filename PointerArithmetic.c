//
// Created by dustyn on 3/23/24.
//

#include <stdio.h>
#include <stdlib.h>

int pointerArithmetic()
{
    const int array[] = {1,2,3,4,5,6,7,8,9,10};
    const int *ptr = array;

   //Here we will just print out the ptr and you will notice something funny, ptr doesn't change at all. This is because we have to iterate through the elements
   //of the array using pointer arithmetic
    for (int i =0;i<10;i++)
    {
        printf("Element : %d: %d\n",i,*ptr);
    }

    //reset pointer to point to first element in array, since array has not changed it is still at element 0, we can just
    //assign ptr to array and it will go back to element 0
    ptr = array;
    printf("We have now reset the pointer to the first element:%d\n",*ptr);

    //We will iterate through the array by iterating the pointer, the pointer just represents the memory address of the current entry in the array
    //Hence ptr++ is the same as going array[x] = array[x+1]  or in this case ptr (remember its at 0 by default) = i !
    for(int i = 0;i<10;i++)
    {
        printf("Element : %d value : %d\n",i,*(ptr + i));
    }
    return EXIT_SUCCESS;
}
