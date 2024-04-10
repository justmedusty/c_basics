//
// Created by dustyn on 4/9/24.
//


#include <sys/stat.h>
#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"

#define BUFSIZ 1024
#define FIFO_NAME "/tmp/my_fifo"


/*
 * A fifo , also known as a named pipe, is another method of achieving IPC. Now a fifo is almost the same as a pipe the main differentiating factor is that fifos have an entry point on disk making them a more
 * persistent method of IPC. This also allows for file permissions to be involved and provides more fine grained access control. A regular pipe just exists in memory and is terminated when the process completes, a fifo
 * has a name and an entry point on disk that persists beyond the life of the process creating it
 *
 * There is no inherent owner of a pipe any processes with the correct permissions can use it
 *
 * They are unidirectional, like a regular pipe
 *
 * They can cause blocking when the buffer is full however this will only affect 1 side of the FIFO
 *
 * Writers block if fifo is full, readers block if fifo is empty
 *
 * Since they have an entry point on disk they make it possible for a variety of other types of processes to access it, making them more interoperable with other languages etc
 *
 * They can be used for communication between unrelated processes which is important because generally pipes can only be shared by related processes such as a parent and child,or just processes with the same process group id
 */
int fifo(){

    //Check if the FIFO exists
    if(access(FIFO_NAME,F_OK) != -1){
        printf("Fifo exists\n");
    }else{
        //Else create it with permission bits 0666 (rw-rw-rw)
        if((mkfifo(FIFO_NAME,0666)) == -1){
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }
    }
    //Here we will fork the process to have the child do the writing and the parent do the reading, fork returns 0 in a child
    if(fork() == 0){
        //This is the child, we will open the fifo with the write only, truncate, and create flags
        int fifo = open(FIFO_NAME,O_WRONLY | O_TRUNC | O_CREAT);

        char msg[] = "This is a fifo, or a named pipe, in action compadre!\n";
        //Attempt to write the contents of msg[] to the fifo, error handling accordingly
        if(write(fifo,&msg,sizeof msg + 1)== -1){
            perror("write");
            exit(EXIT_FAILURE);
        }
        //print a done message, close the fifo and exit
        printf("Child of pid %d done writing\n",getpid());
        close(fifo);
        exit(EXIT_SUCCESS);

    } else{
        //this is the parent
        //Open with read only flag
        int fifo = open(FIFO_NAME,O_RDONLY);
        //buffer to store the data from the fifo
        char msg_buf[BUFSIZ];
        //Print the pid as well as attempt to read from the fifo and error handle accordingly
        printf("Parent preparing to read , PID : %d\n",getpid());
        if (read(fifo,&msg_buf,sizeof msg_buf) == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }
        //Print the message loaded into the buffer and exit on success
        printf("%s",msg_buf);
        exit(EXIT_SUCCESS);



    }



    
}