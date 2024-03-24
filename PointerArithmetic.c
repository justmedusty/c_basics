//
// Created by dustyn on 3/23/24.
//

#include <stdio.h>

int pointerarithmetic()
{
    const int array[] = {1,2,3,4,5,6,7,8,9,10};
    const int *ptr = array;

    for (int i =0;i<10;i++)
    {
        printf("Element : %d: %d\n",i,*ptr);
        ptr++;
    }

    //reset pointer to point to first element in array
    ptr = array;
    printf("We have now reset the pointer to the first element:%d\n",*ptr);

    //using pointer arithmetic to do the same thing
    for(int i = 0;i<10;i++)
    {
        printf("Element : %d value : %d\n",i,*(ptr + i));
    }
}
