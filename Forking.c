//
// Created by dustyn on 4/9/24.
//

/*
 * All processes in UNIX are birthed from a fork(), starting from PID 1, init. You can create your own children. I will show you how to do this.
 * We will also flesh out a little junction where we can push the parent and the child 2 different ways. For this file we will not get into
 * proper IPC techniques I will save this for another file and go into more detail. So we will use a hackjob method of manually comparing against the ppid variable
 * of the parents ppid, of which the child will not have, since the childs ppid will be that the pid of the parent it just forked off of.
 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int fork_intro() {

    /*
     * This is where we will get the ppid of the parent, and how we will work this junction to push each process 2 different ways
     */
    int ppid;
    //The parent process id of the parent is 3148, so we will use this to fork the two processes two different ways
    ppid = getppid();
    printf("%d\n", ppid);

    /*Here we will invoke the fork() system call which will fork the parents to a new child process which will then automatically call exec() on the source code within this file., exec can be used however
     * to execute anything else. For example, when in your terminal you type ls, what is actually happening is your shell whether it be bash, og bourne, C shell etc is that your /bin/sh process is forking itself and then calling exec() on
     * the binary executable found from your env variable. Now this newly forked child will exec the instructions located at the spot where the shell is told "ls" is. This will of course walk the directory and print each file name
     * to stdout and then the forked child of /bin/yourshell will exit and the parent process will likely use an sa handler to call wait() and clean up the child zombie. This happens ad nauseum whenever you type any command into
     * your shell terminal!
     *
     */
    if (fork() == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    /*
     * Here we create a junction to identify both the parent and the child, we can tell which is which by checking the parent process id with getppid system call, the child will have a different ppid from the parent!
     * From here we will just print out a hello message from the parent and the child along with the processes pid from the getpid() system call!
     */
    if (getppid() == ppid) {
        printf("This is the parent\n");
        printf("The parents pid is %d\n", getpid());
    } else {
        printf("This is the child\n");
        printf("The childs pid is %d\n", getpid());

    }
}