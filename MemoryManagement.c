//
// Created by dustyn on 3/23/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int memorymanagement(void) {
    //dynamically allocate memory for the size of an integer
    int *ptr_malloc = (int *) malloc(sizeof(int));

    //If memory not allocated correctly, exit program with exit failure code
    if (ptr_malloc == NULL) {
        printf("Could not allocate memory");
        return EXIT_FAILURE;
    }

    //assign the value of 10 to the address it points to
    *ptr_malloc = 10;
    printf("Dynamic memory allocation using malloc: %d\n", *ptr_malloc);

    //You can also use calloc for allocating memory for arrays
    int *ptr_calloc = calloc(5, sizeof(int));
    if (ptr_calloc == NULL) {
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
    for (int i = 0; i < 5; i++) {
        printf("%d\n", ptr_calloc[i]);
    }
    //we will free the memory we allocated , and then reallocate it with realloc
    free(ptr_malloc);
    ptr_malloc = (int *) realloc(ptr_malloc, 3 * sizeof(int));

    ptr_malloc[0] = 3;
    ptr_malloc[1] = 2;
    ptr_malloc[2] = 1;
    printf("Printing ptr_malloc and realloc\n");
    //To see some undefined behaviour, try changing that 3 to 6, and see what happens
    //when you access memory you did not allocate, the results can be very strange.
    for (int i = 0; i < 3; i++) {
        printf("%d\n", ptr_malloc[i]);
    }

    //Free the memory when we are finished with it

    free(ptr_calloc);
    free(ptr_malloc);
    printf("----------------------\n");

    //A short bit on stack memory
    //The stack is generally fixed size, and the kernel will allocate stack size for each
    //process. You do not need to worry about stack in C generally, however deep recursion
    //can cause issues

    //These 2 are put onto the stack, when this function invocation ends, they will be deallocated autoatically
    int x = 10;

    int y = 150;
    // Unless you are working with a lot of recursion , you will seldom run into problems with stack space.
    return EXIT_SUCCESS;
}


#define MAX_LENGTH 15

//Here is some dynamic allocation for an array no yet assigned values.
int morememorymanagement() {
    int *ptr;
    ptr = malloc(MAX_LENGTH * sizeof(int));
    //Always check that the memory allocation was successful
    if (ptr == NULL) {
        return EXIT_FAILURE;
    }
    //Iterate through the array to the maximum allocated length and assign a value to it and then print it
    for (int i = 0; i < MAX_LENGTH; i++) {
        ptr[i] = i;
        printf("%d \n", ptr[i]);
    }
    //Free the memory now that we are done

    free(ptr);




    //We can also do this navigating struct. This will be a struct Person with name and age and will have two pointers , one to another struct Person, the parent, and also to child.
    //add this in during preprocessing if not declared
#ifndef FAMILY_SIZE
#define FAMILY_SIZE 5
#endif

    //Person struct with some pointers and age and name
    //This can only point to one parent or one child but it is fine as an example
    struct Person {
        char name[15];
        int age;
        struct Person *parent;
        struct Person *child;
    };

    //allocated memory for a family in an array of pointers Person structs
    struct Person *family = malloc(FAMILY_SIZE * sizeof(struct Person));

    //here we will go through and assign name age parents and children
    for (int i = 0; i < FAMILY_SIZE; i++) {
        family[i].age = (i * i);
        strcpy(family[i].name, "BS Name");
        if (i >= 3) {
            family[i].child = &family[0];
            family[i].child = &family[1];
            family[i].child = &family[2];
            family[i].parent = NULL; // May their souls forever rest in peace
            family[0].parent = &family[i];
            family[1].parent = &family[i];
            family[2].parent = &family[i];

        } else {
            //Else they are too young to have a child
            family[i].child = NULL;
        }


    }
    //Now let's traverse through this family and check out some pointer arithmetic, and pull some values out
    for (int i = 0; i < FAMILY_SIZE; i++) {
        if (family[i].parent != NULL) {
            printf("The person of age : %d has a parent of age %d\n", family[i].age, family[i].parent->age);
        } else if (family[i].child != NULL) {
            printf("The person of age : %d has a child of age %d\n", family[i].age, family[i].child->age);

        }
    }

    // We will then free all of that memory
    free(family);

    return EXIT_SUCCESS;
}