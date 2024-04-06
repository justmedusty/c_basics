//
// Created by dustyn on 4/6/24.
//

#include <stdio.h>
#include "HeaderIntroduction.h"
#include "stdlib.h"
#include "string.h"

int main(){
    /*
     * We are just going to instantiate a buffer of size 128 bytes
     * Copy our string into the buffer so that there is something in it
     * We will then print the contents of the buffer
     * Call our function that we included in another file by including the header file
     * with the implementation details defined
     * We will then see that our function successfully clears the buffer on the second fprintf
     * invocation. This is how function , macro, constant exports and imports work in the C language
     */
    char buffer[128];

    char some_string[] = "Hello this is a demonstration";

    strlcpy(buffer,some_string,sizeof(buffer));

    fprintf(stdout,"This is the contents of our buffer: %s\n",buffer);

    function_to_import(&buffer,sizeof buffer);

    fprintf(stdout,"This is the contents of our buffer now that we have used our imported function: %s\n",buffer);

    return EXIT_SUCCESS;

}