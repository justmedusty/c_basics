//
// Created by dustyn on 3/23/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    //dynamically allocate memory for the size of an integer
    int* ptr_malloc = (int*)malloc(sizeof(int));

    //If memory not allocated correctly, exit program with exit failure code
    if (ptr_malloc == NULL)
    {
        printf("Could not allocate memory");
        return EXIT_FAILURE;
    }

    //assign the value of 10 to the address it points to
    *ptr_malloc = 10;
    printf("Dynamic memory allocation using malloc: %d\n", *ptr_malloc);

    //You can also use calloc for allocating memory for arrays
    int* ptr_calloc = calloc(5, sizeof(int));
    if (ptr_calloc == NULL)
    {
        printf("Could not allocate memory");
        free(ptr_malloc);
        return EXIT_FAILURE;
    }
    //assign some values to the pointed to addresses
    ptr_calloc[0] = 1;
    ptr_calloc[1] = 2;
    ptr_calloc[2] = 3;
    ptr_calloc[3] = 4;
    ptr_calloc[4] = 5;

    printf("Printing ptr_calloc array\n");
    for (int i = 0; i < 5; i++)
    {
        printf("%d\n", ptr_calloc[i]);
    }
    //we will free the memory we allocated , and then reallocate it with realloc
    free(ptr_malloc);
    ptr_malloc = (int*)realloc(ptr_malloc, 3 * sizeof(int));

    ptr_malloc[0] = 3;
    ptr_malloc[1] = 2;
    ptr_malloc[2] = 1;
    printf("Printing ptr_malloc and realloc\n");
    //To see some undefined behaviour, try changing that 3 to 6, and see what happens
    //when you access memory you did not allocate, the results can be very strange.
    for (int i = 0; i < 3; i++)
    {
        printf("%d\n", ptr_malloc[i]);
    }

    //Free the memory when we are finished with it

    free(ptr_calloc);
    free(ptr_malloc);
    printf("----------------------\n");

    return EXIT_SUCCESS;
}
