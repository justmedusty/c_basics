//
// Created by dustyn on 3/28/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//define max thread count
#define THREAD_COUNT 10

//This is our global variable
int counter = 0;
//This is our mutex, a mutex is one of the answers to the problem of concurrent resource access.How do you manage resource access
//between many different processes? mutexes is one of the answers to that question. I will show you below
pthread_mutex_t mutex;

//This is the function that each concurrent thread will execute, they will each access the global variable counter, not very safe! Sounds like race conditions to me. We can fix that with a mutex.
void *thread_function(void *arg) {
    int i;
    //We will lock our mutex, disallowing other processes and threads from accessing this function until it is unlocked
    pthread_mutex_lock(&mutex);
    //very simple loop that just increments counter 10 times
    for (i = 0; i < 10; i++) {
        counter++;
        printf("Thread %d: Counter value %d\n", *((int *) arg), counter);

    }
    //Now we will unlock the mutex, allowing another thread to take the lock and increment the counter itself
    pthread_mutex_unlock(&mutex);
    //Terminate the thread, note this does not handle any mutexes, semaphores, fds, etc so it is important to handle these things BEFORE you call pthread_exit()
    pthread_exit(NULL);
}

int main() {
    //posix thread array of length THREAD_COUNT specified above (we chose 10 for this)
    pthread_t threads[THREAD_COUNT];
    //The thread argument array, this just tells the thread function what number this thread has been assigned
    int thread_args[THREAD_COUNT];
    //The integer we will use for looping and incrementing
    int i;

    //We will attempt to init the mutex with no attributes hence the NULL argument, do your standard error checking.
    //Most things return -1 on error however these pthread system calls will return 0 on success and on error return a specific error number
    //We can handle the specific errors if we want but this is just an introduction to mutexes , so we will just do basic handling
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr,"Mutex init failed\n");
        exit(EXIT_FAILURE);
    }

    //Here we will create our THREAD_COUNT (10) posix threads, and slap a number on them, so we know which thread is which. We will feed the start routine parameter the thread_function parameter above which will
    //showcase the magic of the mutex
    for (i = 0; i < THREAD_COUNT; i++) {
        thread_args[i] = i;
        //NULL attribute cause we don't need it, thread at i spot in array ,start thread_function with args thread_args (just the number i to number the thread for id purposes)
        //standard error processing of course
        if (pthread_create(&threads[i], NULL, thread_function, (void *) &thread_args[i]) != 0) {
            fprintf(stderr, "Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    //Now we will utilize the pthread_join call which just returns 0 once the specified thread has terminated , so that we can keep track of their termination and make sure that they terminated properly
    //Again, there are specific errors that can occur, and it will return specific error numbers
    //For this, we don't care. Just handle errors in the most basic way possible.
    for (i = 0; i < THREAD_COUNT; i++) {
            if (pthread_join(threads[i], NULL) != 0) {
                fprintf(stderr, "Error joining thread");
                exit(EXIT_FAILURE);
            }


    }

    //Destroy the mutex, we are done. Cool, right?
    pthread_mutex_destroy(&mutex);
    exit(EXIT_SUCCESS);

    //Now when you run this , you will notice that there are no race conditions! That is because once a thread holds the mutex, no other thread can access that function and touch that global variable (counter)
    //This functionality is absolutely VITAL because race conditions can lead to some very unwanted behaviour, and this is just one of the ways you can solve that problem !
    //You might also see the scheduler in action, the kernel is going to decide who gets the CPU time and when, the threads accessing the function and taking the mutex don't exactly do so in a chronological order. It is a little out of whack.
    //This is the scheduler in action! One of the many cool things the kernel does for us that we take for granted.
}