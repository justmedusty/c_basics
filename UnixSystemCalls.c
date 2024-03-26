//
// Created by dustyn on 3/25/24.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h> // for kill() system call

//We will get into some System calls here. System calls are the layer of asbtraction between the user-space and the kernel space how a program can interface with the linux kernel and
//elevate the execution of a function into the mythical kernel-space


//A system call we can start with, an easy one, is exit(2)
//_exit() will ask the kernel to kill the calling process immediately, any open fds and any children are immedately killed amd the parent from which
//this process was forked receives a SIGCHILD signal


int exitSysCall(){
    printf("This is a heavy handed way to kill a process IMMEDIATELY");
    //Notice this will just immediately exit, this is because the output of printf in C is buffered and exit system call kills this process IMMEDIATELY
    //We can remedy this problem by flushing stdout!
    //Now it prints before the kernel is able to kill the process! It will immediately empty instead of waiting for the buffer to finish iterating and spitting out every character!
    fflush(stdout);
    //0 because this is on purpose
    _exit(0);
}

//Another way we could accomplish this is by utilizing the system calls getpid() and kill()
//This is how you can get the PID of this current process from the kernel, and then request the kernel to kill it! Process suicide , if you will

int pidAndKill(){
    //Utilizing the getpid() system call to get our process id
    int pid = getpid();
    //printing to console
    printf("The PID of this Process is %d",pid);
    //We must again flush stdout due to the buffered nature of printf in C
    fflush(stdout);
    //Now watch this process plead to the kernel for a quick , merciful death (non error signal 1)
    kill(pid,1);

}

int forkingandwaiting(){
    //Let's get these onto the stack for some basic math
    signed int a = 5, b = 10;
    printf("%d %s %d equals %d !\n",a,"plus",b,a+b);
    printf("%d %s %d equals %d !\n",a,"minus",b,a-b);
    printf("%d %s %d equals %d !\n",a,"multiplied by",b,a*b);

    //The fork is very important in UNIX. The init process is the mother of all processes in UNIX, forking itself. EVERY SINGLE PROCESS CAN BE TRACED BACK TO INIT BY ITERATING THROUGH PPIDs !
    pid_t pid = fork();

    //If error code
    if (pid < 0){
        perror("An error occured\n");
        //We will flush stdout just in case
        fflush(stdout);
        //Ask the kernel to exit immediately with exit code EXIT_FAILURE (1)
        _exit(EXIT_FAILURE);
    }
    //If good (0)
    else if(pid == 0){
        printf("Child executing\n");
        //flush because these _exit syscalls are much faster than the printf buffer
        fflush(stdout);
        //simulate a longer execution
        sleep(3);
        _exit(EXIT_SUCCESS);
    }
    else{
        printf("Waiting for child execution to complete\n");
        int status;
        //Wait for the status of the execution, if there was an error print err child failed and exit on EXIT_FAILURE (1) code
        pid_t child = wait(&status);
        if(child == -1){
            perror("Child Failed\n");
            _exit(EXIT_FAILURE);
        }
        //Let the user know that the almighty kernel(alhamdulilah) has terminated the child process
        printf("Child process with pid %d was terminated by the almighty kernel\n",child);
    }

    printf("Parent exiting...\n");

    //Exit on Exit Success (0)
    _exit(EXIT_SUCCESS);





}

