//
// Created by dustyn on 4/12/24.
//


#include <unistd.h>
#include <sys/wait.h>
#include "stdio.h"
#include "stdlib.h"


/*
 * Just for shits and gigs
 */

int forkbomb(){

    for(;;){
        int childPid = fork();

        if(childPid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if(childPid == 0){
            forkbomb();
        }
        if(childPid > 0){
            //Take this out for a real bomby bomb
            wait(NULL);
            EXIT_SUCCESS;
        }


    }
}