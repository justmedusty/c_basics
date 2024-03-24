//
// Created by dustyn on 3/22/24.
//
#include<stdio.h>
#define TESTFUNCTION
#define NUMBER 3
#define MULTIPLY(x) ((x) * (x))
int preprocessordirectives(void)
{
    //This will only compile if TESTFUNCTION is defined above
#ifdef TESTFUNCTION
    printf("This only compiles if TESTFUNCTION is defined.\n");
#endif


    //Only compile if NUMBER is defined
#ifdef NUMBER
    //Only compile if number is not divisible by 2
    #if NUMBER % 2
        printf("Number is odd\n");
    //only compile if it is
    #elif !(NUMBER % 2)
        printf("Number is even\n");

#endif

#endif

   //if multiply macro is defined, print the result of the macro fed the value 5
#ifdef MULTIPLY
    printf("%d\n",MULTIPLY(5));
#endif

//If not defined, define this before compilation
#ifndef ADD
    #define ADD(x,y) ((x) + (y))
#endif

    //Then I will be able to use this macro even though i did not define, it will be defined ta compile time in the preprocessing stage
    printf("%d\n",ADD(5,4));

    return 0;
}

