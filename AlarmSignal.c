//
// Created by dustyn on 4/8/24.
//


/*
 * There are many signals in the UNIX which we will get into over many different files in this repo. One of the files we have already seen is the SIGCLD signal in our stream server to let us know a child has died which can use
 * to come in and clean up the process to not leave zombie processes around. The SIGALRM signal is something we can set and which will schedule a SIGALRM signal in X seconds which we can then handle
 * using signal() system call. There are many familiar things that are just using a SIGALRM signal under the hood
 * Things that make use of SIGARLM
 *
 * Network timeouts
 * I/O timeouts
 * Real time processing
 * Timers in programs
 * System call timeouts
 *
 */

#include <string.h>
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "signal.h"
#include "errno.h"

/*
 * This is our handling function all it will do is set another alarm and print how many seconds have passed
 */
void sigalrm() {
    alarm(5);
    printf("5 seconds has passed\n");
}

int alrmsigdemo() {

    //We will build a simple sigaction struct that will have a signal action handler of our sigalrm which will just tell us how many seconds it has been and also set another alarm
    struct sigaction action;
    action.sa_handler = sigalrm;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;



    //For this demo we will build a simple timer that lets us know everytime 5 seconds passes
    alarm(5);

    //We will our sigaction struct to handle SIGARLM signals and the action will be our sigalrm() function above which will just reset the timer and print how many seconds passed
    if (sigaction(SIGALRM, &action, NULL) == -1) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    for (;;) {
        //The pause system call is for sleeping to wait for a signal, to either handle the signal or to wait for termination in the case of a SIGKILL signal
     pause();
    }

    return EXIT_SUCCESS;


}