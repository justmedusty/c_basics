//
// Created by dustyn on 4/14/24.
//

/*
 * You have seen some heap allocation via the likes of malloc, calloc and reallc but you can actually allocate more memory on the stack
 * Some benefits to stack allocation over heap allocation are as follows:
 * No need to free()
 * allocates memory faster because the compiler handles this inline and directly adjusts the stack pointer
 * It can also be used for non-local gotos from signal handlers to help avoid memory leaks since the stack frame pointer resets to the end of the previous frame when a function returns
 *
 * Stack allocation is done with the alloca() function.
 *
 */

#include "stdlib.h"
#include "stdio.h"

int someFunc(int x, int y){
    void *stackAlloc;
    //Allocation 1024 bytes on the stack and return a pointer to the memory, this will be gone when this function returns since so there is no need to free it.
    stackAlloc = alloca(1024);
    int ret = x * y;
    return ret;
}

int stackalloc(){

    int ret;
    ret = someFunc(5,8);
    printf("%d\n",ret);

    return EXIT_SUCCESS;
}

