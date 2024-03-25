//
// Created by dustyn on 3/25/24.
//
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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