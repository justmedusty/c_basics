//
// Created by dustyn on 4/5/24.
//

/*
 * In C our streams are almost always buffered, there are 3 types of buffering. Full buffering, line buffering, and no buffering. Full buffering means I/O does not happen until the buffer is full, line buffering
 * means that the new line character will cause the buffer to be flushed, and no buffering means direct I/O. This can be manipulated in C using setbuf or setvbuf. setvf buf has some bitmask flags which can be used to modify
 * a steams buffer behavior. With setbuf we can pass our own buffer , this generally will need to be of size BUFSIZ which is a predefined constant
 *
 * Note: Unbuffered streams are generally not wanted except in more specific situations since it will call a read and write for every single character that enters the stream which as you can imagine,
 * is not the most performant in a lot of common scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int setbuf_and_setvbuf(){

    //Our very own buffer for I/O streams
    char buffer[BUFSIZ];

    //We will change the buffering behaviour of the stdout stream because why not, that is fun
    //With this invocation, stdout will now be fully buffered using our own buffer, it is fully buffered by default
    //except in a terminal environment where it is line buffered by default
    setbuf(stdout,buffer);

    //We can use setbuf to turn buffering off entirely too! Just pass NULL for the buffer

    setbuf(stdout,NULL);
    //What is interesting out no buffering is with systemcalls like _exit(), which normally would not flush the steam buffers, it is not necessary to do so ! This means you can do this, which normally will
    //not print because the streams are not flushed on _exit() call

    printf("You will see this now without flushing stdout!");
    //_exit(EXIT_SUCCESS);
    //To prove my point, you will need to comment and uncomment this but check this out

    //We are using setvbuf to set the flag bitmask of stdout to _IOFBF which means we are back to the default (fully buffered)
    if(setvbuf(stdout,buffer,_IOFBF,BUFSIZ) != 0){
        //This may set errno so we will print the error it is holding
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    //Now you will see that this will not work due to the lack of flushing done when directly calling the system call _exit() as
    //opposed to exit() which does handle flushing for you

    printf("You won't see this because stdout is now fully buffered and _exit does not flush your streams!");
    _exit(EXIT_SUCCESS);

    /*
     * It is more laborious for the kernel when we use unbuffered streams however they have their place, and you can see a difference here when we use printf which goes through the fully buffered stdout stream,
     * anything that does not flush your streams automatically you would not see the print message and would not manually flush it yourself!
     */



}