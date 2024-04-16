//
// Created by dustyn on 4/16/24.
//

/*
 * This is a very simple program that will use the kernel interface to write our string from our buffer directly to stdout and exit with the _exit system call.
 * We can use the gcc -S flag during compilation to actually generate the assembly file and inspect the contents of it! It will be fairly short since we are
 * using raw system calls here. If you use library functions like printf() there will be a lot more going on.
 */
#include "unistd.h"

int main(){
    char buff[] = "Hello world";
    write(1,&buff,sizeof buff );
    _exit(0);
}