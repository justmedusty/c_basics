//
// Created by dustyn on 3/29/24.
//
//Semaphores are another way to handle concurency is unix/posix compliant systems
//Semaphores are just integers and can be decremented and incremented
//When they are 0, there will be a block on the resource until the current thread or process
//increments the sempaphore
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <error.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
//15 threads for this bad boy
#define THREAD_COUNT 15
//What sets apart semaphores from mutexes is that mutexes are binary whereas semaphores are integers which can allow many accesses but up to the limit of the semaphore's value
//Want to test this out? Change this to 3, the program will fail !
#define SEMAPHORE_INIT_VAL 2

//We'll define another thread routine pointer function with a global variable!

//The number 7 , we will say that this is a 1 byte number (it is a 4 byte number but we will pretend). If the size goes over 1 byte, we have failed and must hang our heads in shame
int binary = 0b00000111;

//Our actual semaphore here
sem_t semaphore;

//The purpose of this function is to showcase that my chose value of 2 processes accessing this global variable at once is crucial, if a third joined, the bits would be shifted right out of the 8 bits above. We will do a check and exit if these condtions are met!
//They will not be met because the semaphore only allows concurrent access for 2 threads
void *thread_routine(void *arg) {


    //This will automatically decrement the semaphore for us, and if the semaphore value is currently 0 , the thread must wait until it is of non-zero value
    sem_wait(&semaphore);

    //We will shift binary 2 bits to the left and print a message
    binary = binary << 2;
    printf("Bit shift! Binary is now %d !\n Thread %d is sleepy...\n", binary, *(int *)arg);

    //If a third thread enters, the value may go above 256 in which case all bits have shifted into the void and we must shut the program down immediately
    if (binary > 256) {
        fprintf(stderr,"The semaphore has failed ! Too much concurrent access has taken our binary number out of bounds !\n");
        exit(EXIT_FAILURE);

    }

    //Sleep for 2 seconds (2mil microseconds) then print a new message to signify this threads slumber is over
    usleep(2000000);
    printf("Thread %d has awoken from its slumber!\n",  *(int *)arg);

    //Now let's shift those bits right back
    binary = binary >> 2;
    printf("Bit shift! Binary is now %d !\n Thread %d will now be destroyed\n", binary,  *(int *)arg);


    //This will increment the semaphore , if the current value is 0, the thread waiting at sem_wait that is in a state of blocking will awaken and execute
    sem_post(&semaphore);



    //Terminate the thread
    pthread_exit(NULL);
}


int main(){
    //posix thread array of length THREAD_COUNT specified above (we chose 15 for this)
    pthread_t threads[THREAD_COUNT];
    //The thread argument array, this just tells the thread function what number this thread has been assigned
    int thread_args[THREAD_COUNT];
    //The integer we will use for looping and incrementing
    int i;

    //We will attempt to initialize the semaphore, it will be of value semaphore init val (2) and the 0 for pshared indicates that this is shared amongst THREADS not PROCESSES,
    //If we want to share amongst processes we would turn that 0 to a 1.
    //Do error checking accordingly
    //On error sem_init will set errno so we will print out errnos new value on failure
    if (sem_init(&semaphore,0,SEMAPHORE_INIT_VAL) != 0) {
        fprintf(stderr,"Semaphore init failed with error %d\n",errno);
        exit(EXIT_FAILURE);
    }

    //We will increment through each pthread array and create a pthread and send it right to our thread_routine function with the arg i which will be the thread identifier
    for(i =0;i<THREAD_COUNT;i++){
        thread_args[i] = i;
        //We will cast thread args to a void pointer to indicate any type which we specified above, handle errors accordingly
        if(pthread_create(&threads[i],NULL, thread_routine,(void *)&thread_args[i]) != 0){
            fprintf(stderr,"Error creating thread %d ! \n",i);
            exit(EXIT_FAILURE);
        }
    }
    //Now we will call pthread_join for each thread and pthread_join waits for confirmation of termination we know things are being cleaned up properly, if not exit the program !
    for(i=0;i<THREAD_COUNT;i++){
        if(pthread_join(threads[i],NULL) != 0){
            fprintf(stderr,"Error occurred joining thread %d !\n",i);
            exit(EXIT_FAILURE);
        }
    }
    //Destroy the semaphore, we are done with it
    sem_destroy(&semaphore);
    //Exit on success!
    exit(EXIT_SUCCESS);


    //Wow , you're a real unix programmer now huh?
    //So in retrospect, semaphores and mutexes accomplish the same goals albeit in different ways. A semaphore can allow multiple concurrent accesses either by other threads or other processes
    //Whereas a mutex is binary and only 1 access is permitted at a time!

}