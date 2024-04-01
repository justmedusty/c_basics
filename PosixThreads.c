//
// Created by dustyn on 3/29/24.
//


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//define the number of threads we're going to use
#define MAX_THREADS 5

//This is the function that we will pass in pthread_create in order to showcase how posix threads work. It will just
//show you how each thread is created, passed a function to execute , and then how we handle the cleanup after It will just print
//The number of which the thread was assigned at birth and immediately exit
void *thread_function(void* arg){
    printf("This is thread %d\n", *(int*) arg);
    pthread_exit(NULL);
}

//Our main method from which the magic happens baby!
int posixThreads(){
    //We will instantiate our threads at size MAX_THREADS (5) as defined above
    pthread_t threads[MAX_THREADS];
    //This will just be the number we assign each thread chronologically
    int arg[MAX_THREADS];
    //We will iterate through each element in our array of type pthread_t, and attempt to create a thread and pass our pointer function and the arg(just the thread number
    //This will print out the thread name before exiting
    for(int i =0;i<MAX_THREADS;i++){
        //increment arg and assign the value of i each iteration
        arg[i] = i;
        //Standard error checking, pthread_create returns 0 on success, handle accordingly
       if (pthread_create(&threads[i],NULL, thread_function,&arg[i]) != 0){
           perror("error creating thread!\n");
           exit(EXIT_FAILURE);
       }



    }
    //Print a message once all threads have been created!
    printf("All threads created\n");

    //Now this is important since the pthread_join system call waits to ensure that the threads have properly terminated and awaits such
    //On error we will get a non zero return value, handle accordingly
    for(int i = 0;i<MAX_THREADS;i++){
        if(pthread_join(threads[i],NULL) != 0){
            perror("Error joining thread\n");
            exit(EXIT_FAILURE);
        }

    }
    //That's it! Now you know how posix threads work! now you can either copy my code and try it out or go write your own program that utlizes
    //posix threads !
    printf("All threads terminated!");
    return EXIT_SUCCESS;

    //You're a real unix programmer now!


}