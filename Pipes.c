//
// Created by dustyn on 4/9/24.
//

/*
 * Pipes are another method of IPC, inter-process communication. Pipes are unidirectional with a head and an ass-end. One for reading and one for writing.
 * Data written to the read end is buffered by the kernel until it is read from the read end.
 */

#include <sys/wait.h>
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define BUFSIZ 1024

int pipes() {

    //Our message to be written into the pipe
    char message[] = "Pst I am the parent and I am talking to you my child.";

    //Our buffer for storing read data in the child process
    char buf[BUFSIZ];

    //Our 2 pipe file descriptors, note there can only be 2 since there is just 2 ends 1 reading end 1 writing end
    int pipefd[2];

    //This will store the result of fork which will inform us of which process this is, parent or child, we can create a junction and push each one a different way
    pid_t cpid;

    //Create the pipe, error check accordingly
    if (pipe(pipefd) != 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //We will make sure to identify the process by assigning the result of fork to our cpid. Fork will return 0 in the child and the child pid in the parent so we can
    //identify each process this way.
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    //The child will read the pipe, we assure this is the child by checking for 0 which indicates child process
    if (cpid == 0) {

        //Close the write end since we don't need it
        close(pipefd[1]);

        //Read until there is an error or nothing is left, note a read will block so this will stay blocking until it is finished or there is any error
        while (read(pipefd[0], &buf, 1) > 0) {
            write(STDOUT_FILENO, &buf, 1);
        }

        // Once the read has completed we can write a line break to std out file descriptor
        write(STDOUT_FILENO, "\n", 1);

        //We can now close the read end of the pipe
        close(pipefd[0]);

        //And exit
        _exit(EXIT_SUCCESS);


        //else the parent write our hardcoded message into the pipe
    } else {

        //Close the read end of the pipe , we do not need it
        close(pipefd[0]);

        //Write the message to the write end of the pipe
        write(pipefd[1], message, strlen(message));

        //This will send an EOF to the reader
        close(pipefd[1]);

        //Wait for the child to finish
        wait(NULL);

        //Exit !
        exit(EXIT_SUCCESS);
    }
}

