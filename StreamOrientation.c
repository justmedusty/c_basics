//
// Created by dustyn on 4/5/24.
//

/*
 * A streams orientation refers to whether it is going byte by byte (such as for ASCII)
 * ,multibyte ( such as UTF-8 or UTF-16) since there are so many chinese characters. The terms here are byte-oriented and wide-oriented We can actually decide which to use in C. There is
 * also a third option of unspecified. We can actually change the stdout stream to be wide oriented using fwide
 */

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
int StreamOrientation(){

    //This will make our stream byte oriented
    fwide(stdout,-1);

    //Now this will make our stream wide oriented if positive value passed for mode, byte oriented if pass negative value passed,and unspecified if 0\
    //Making stdout wide-oriented may cause some different behaviours when dealing with things like accented characters, or things like chinese symbols since these are larger than 1 byte and use their
    //own encoding usually UTF-8 or UTF-16
    fwide(stdout,1);

    return EXIT_SUCCESS;

}

