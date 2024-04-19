//
// Created by dustyn on 4/19/24.
//

/*
 * Each open file descriptor in a process has a position in said file.
 * We can use lseek() to achieve this. We will create a new file and write a message
 * to it. We will then get the byte offset with the lseek() system call combined with the SEEK_CUR
 * flag.
 */

#include <stdlib.h>
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"

int lseek_demo(){

    int fd = open("/home/dustyn/myfile.txt",O_RDWR | O_CREAT | O_TRUNC);
    off_t file_offset;
    char buf[] = "hello";
    write(fd,&buf,sizeof buf);

    if(fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    file_offset = lseek(fd,0,SEEK_CUR);

    if(file_offset == -1){
        perror("lseek");
        exit(EXIT_FAILURE);
    }

    printf("The current byte offset in the file is %lo bytes\n",file_offset);

    /*
     * We can also reset our byte offset by going to 0(long int , typically 8 bytes but your mileage may vary)
     * This will modify the entry in the file table in the file descriptor table of this process in the kernel.
     */
    file_offset = lseek(fd,0,0L);
    printf("The current byte offset in the file is %lo bytes\n",file_offset);
}