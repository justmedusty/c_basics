//
// Created by dustyn on 4/16/24.
//

/*
 * In UNIX systems each process has its own entry in the process table which is where the kernel stores information about each process.
 * One of these tidbits of information is the controlling terminal (tty) of a process. We can have a look here and find it using
 * tcgetpgrp(). We will be looking for the process group id that controls the tty1 instance
 */

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "errno.h"


int main(){
    pid_t pid = getpid();
    pid_t pgrp = getpgid(pid);
    int fd = open("/dev/tty",O_RDWR);

    if(fd==-1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    pid_t controlling_terminal = tcgetpgrp(fd);

    if(controlling_terminal == -1){
        perror("tcgetpgrp");
        exit(EXIT_FAILURE);
    }
    /*
     * It will be different for everybody but in my case /dev/tty1 is controlled by pgrp 1973 which is a /bin/bash child of my run desktop script.
     * You can see the process tree in your system with the pstree command in the terminal.
     */
    printf("Process %d of group %d has a controlling terminal process group id of %d\n",pid,pgrp,controlling_terminal);

    exit(EXIT_SUCCESS);
}
