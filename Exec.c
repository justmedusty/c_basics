//
// Created by dustyn on 4/12/24.
//


/*
 * Exec is what allows a process to execute instructions held somewhere, so when a process is forked, a child is created that is essentially a copy of the
 * parent. But what if we want some entirely unrelated instructions executed? This is where exec comes in! This is how a shell executes commands you type, it forks and the child execs the executable for that command!
 * Here we will do some execing here and you can see the unix exec calls in action! The child will have the same tty instance we can see the output from the parents tty session. This is because tty sessions are attatched to a process group, not an
 * individual process.
 */


#include <sys/wait.h>
#include "unistd.h"
#include "stdlib.h"
#include "stdio.h"


int main(int argc,char *argv[]){
    /*
     * Make sure a command is specified and alert the user
     * of proper usage of this program
     */
    if(argc < 2){
        fprintf(stderr,"usage : exec command (opt: args)");
        exit(EXIT_FAILURE);
    }
    //Will hold the ret val of fork, 0 on child, pid on parent, -1 on err
    int childpid;

    //The command to be execd
    char *command = argv[1];

    //The path, generous with the size here
    char command_path[256];

    //Use snprintf to write the full path and append the command to the end of it
    snprintf(command_path, sizeof(command_path), "/bin/%s", command);

    //Check X_OK flag to make sure we can exec this file
    if(access(command_path,X_OK) == -1){
        fprintf(stderr,"Command %s not found !\n",command);
        exit(EXIT_FAILURE);
    }

    //Fork it!
    childpid = fork();

    //Error handle accordingly
    if(childpid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    //If it's the child , call execve and pass the full path, the args, and no env needed since we're just using the bin dir path directly
    //perror on error and exit failure
    if(childpid == 0){
        //this is the child
        execve(command_path,&argv[2],NULL);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    //Parent waits for child to complete and then exits
    if(childpid > 0){
        wait(NULL);
        exit(EXIT_SUCCESS);
    }








}