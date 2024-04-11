//
// Created by dustyn on 4/11/24.
//

/*
 * Process groups in unix a group of one or more processes, the "process group lead" would be the process whos process id matches the process group id
 * We will go over how these are created here and how to view them. these values are stored in the process table in unix. The parent will be the group leader as you can tell, however upon
 * death the pgroup id does not change in the group. You can use setpgid() to change it if you desire, the main point of the process group id is to control access in a terminal session as well
 * as handling signals for said terminal session. In the process table in UNIX, a tty instance is tied to a process group not by an indivual process.
 *
 * Another concept we will see here is orphaning. When a parent is killed, the child has now become an orphan. When this happens, the new parent id will be that of init, the father of all processes with pid
 * of 1. We will see this in action when the parent terminates before the child and the child prints the ppid, it will be 1!
 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main() {


    int childpid = fork();

    //Exit the parent process
    if ((childpid) > 0) {

        printf("This is the parent process %d with process group ip of %d\n", getpid(), getpgid(getpid()));

        exit(EXIT_SUCCESS);

    //this is the child
    } else if (childpid == 0) {

        printf("This is the child process %d with process group ip of %d\n", getpid(), getpgid(getpid()));

        printf("Now that the child is orphaned, the parent id will be that of the init process! check this out: parent id = %d\n",getppid());

        //We can set out pgroup id and change it to ourselves here!
        setpgid(getpid(), getpid());

        printf("We have now set our own pgid to make ourselves the group leader! New pgroup id is %d\n",getpgid(getpid()));

        exit(EXIT_SUCCESS);

    } else {

        perror("fork");

        exit(EXIT_FAILURE);

    }


}