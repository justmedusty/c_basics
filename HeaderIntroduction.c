//
// Created by dustyn on 4/6/24.
//

#include <memory.h>
#include "HeaderIntroduction.h"

/*
 * This is a very simple function that will just take a pointer to something we wanted zerod using memset, very simple
 * however defining interfaces in header files is very important in C as this is how you can make sure you follow the
 * implementation guidelines to make sure you are using the function properly, we can jump over to HeaderIntroduction.h to define
 * how to use this function.
 */
void function_to_import(void *thing_to_be_zerod ,size_t size){

    /*
     * Just set the memory across the specified size of this object of any type to 0, just to demonstrate
     * using header files in C and importing and using the functions after.
     */
    memset(thing_to_be_zerod,0,sizeof thing_to_be_zerod);

}