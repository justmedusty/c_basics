//
// Created by dustyn on 4/14/24.
//

/*
 * Gotos a method of conditional logic that you can employ in C , they let you define code snippets inside of a function and let you jump to a certain snippet under certain conditions
 * In the below code we will just have a simple example of gotos and as you can see, we can jump around inside a function using gotos.
 */
#include "stdlib.h"
#include "stdio.h"

int gotos(){

    int x;

    //scan stdin for input
    scanf("%d",&x);\

    int result;

    if(x == 1){
        goto squared;
    }

    if(x == 2 ){
        goto add;
    }

    if(x != 1 && x != 2){
        exit(EXIT_FAILURE);
    }


    squared: result = x * x;


    add: result = x + x;


    printf("%d",result);


}