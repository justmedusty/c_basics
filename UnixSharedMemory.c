//
// Created by dustyn on 4/12/24.
//

#include <fcntl.h>
#include "stdlib.h"
#include "unistd.h"
#include "stdio.h"
#include "sys/mman.h"
#include "sys/shm.h"
#include "sys/ipc.h"
#include "sys/types.h"
#include "sys/wait.h"
/*
 * Shared memory is another method of IPC in a UNIX environment. It is persistent, it lives in the kernel until the system is shut down.
 * Generally it is required that the access be coordinated via a semaphores or maybe a mutex. Here we will just brush over the basics we won't initialize a semaphore for this one
 */

#define SHMSIZE 1024

int shared_memory(){

    //Shared memory id
    int shmid;
    //our key, this is used to access IPC objects
    key_t key;
    //This will point to memory in this current process where the shared memory has been mapped
    char *shm_pointer;
    //This will store the result of the fork() call
    pid_t pid;

    //Here we will actually create the file which we will use as our shared mem, use CREAT and TRUNCflags for create if not exists,if exists truncate to 0 bytes
    int shm_file = open("/tmp/shmdemo",O_CREAT | O_TRUNC ,0644);
    //Here we will create a key using the ftok function, we will supply the shm file as well as a proj id which is just a single char to identify this usage of the space
    key = ftok("/tmp/shmdemo",'D');
    //Close since we only needed to create the file if it didn't exist
    close(shm_file);

    //Make sure key does not hold an error value
    if(key == -1){
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    //Now we will create our shared memory with shmget, passing our key, size of shared memory (1024 bytes here), IPC_CREAT to create it, with permission bits 0664(rw-rw-r)
    //There is no X bits here since they are not required by the system
    shmid = shmget(key,SHMSIZE,IPC_CREAT | 0664 );

    //Check for error in the function response
    if(shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    //This will point to the newly mapped memory in this functions address space. This makes access very fast since it is mapped directly
    //into virtual memory via the page tables
    shm_pointer = shmat(shmid,NULL,0);

    //Make sure the response was not -1, must cast to char pointer since shm_pointer is a pointer to a char
    if(shm_pointer == (char *) -1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }


    //Fork it!
    pid = fork();

    //Make sure fork did not fail
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);

    }else if (pid == 0){
        //child process

        //The child will write into the shm_pointer memory space and leave the message "The child was here"
        printf("Child getting ready to write to shared memory space\n");
        sprintf(shm_pointer,"The child was here");

    }else{
        //parent process

        //Wait for child to finish with the wait() call, to handle the sigcld signal
        wait(NULL);

        //Now print out whats in the memory space mapped in the parent process
        //The child message is there!
        printf("Parent has retreived the following the shared memory : %s\n",shm_pointer);
    }
    //We will use shared memory control to remove the shared memory id with flag IPC_RMID, pass null for buff since it is not required
    //standard error checking
    if(shmctl(shmid,IPC_RMID,NULL) == -1){
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    //Now we will detach the process local memory mapping, error handle accordingly
    if(shmdt(shm_pointer) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    //Thats it! That's a simple example of shared memory in UNIX systems!

    return EXIT_SUCCESS;


}