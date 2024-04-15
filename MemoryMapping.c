//
// Created by dustyn on 4/15/24.
//


/*
 * The way memory works in UNIX is you have page tables where virutal memory is mapped to physical memory. There is usually a disconnect between a process and and physical memory.
 * There are a few reasons for this. One reason for this is the ability to hold more in the page tables than exists in physical memory, this is what swap memory is. When a process tries to access memory via its page table mappings and the kernel
 * discovers that said memory does not physically exsit, ther kernel will then map that area to disk (swap space) instead, allowing the process to use more memory than is physically available. Another big reason is security, creating a layer of abstraction
 * between the physical memory and a process is important as by accessing raw physical memory an attacker could know exactly where to look to find executable instructions or access sensitive data. The page tables create a layer of abstraction  that will
 * randomize the virtual mapping to mask where the memory addresses physically are in the system. This is very important. Virtual memory mappings also help the kernel enforce permissions. This is because the page tables are orchestrated and maintained by the kernel,
 * whereas physical memory is not. This gives the kernel power to enforce group/user specific permissions on memory addresses. Otherwise it would be the wild west! That would be bad! Each time a process is created it gets it's own private address range in the page tables
 * so it has its own memory space. What I will show you today is how to map file data into a processes memory range in the page tables which will greatly improve read and write speeds!
 */


#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "string.h"

int main(){
    //The address which will hold the pointer to our newly mapped memory
    char *address;
    //Fd of the file we're going to work with
    int fd;
    //This will hold information about the file, we will need the length of the file.
    struct stat sb;

    //Here we will just create a text file, with flags create if non exists, read/write, and trunc to 0 bytes if file exists
    fd = open("/home/dustyn/myfile.txt",O_CREAT | O_RDWR | O_TRUNC );
    //error handle
    if(fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    //Here we'll get some user input to write to the file before we map it into memory
    char file_contents[100];
    printf(" Enter your message to write to the new file to be memory mapped : ");
    scanf("%s",file_contents);

    //Write to the file with the input just given by the user, error handle accordingly
    if (write(fd,file_contents, strlen(file_contents)) == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }
    //Call fstat to get the file information about the file we'll be needing the size of the file
    if(fstat(fd,&sb) == -1){
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    //Now we will use our address pointer and assign it the return value of memory map. This maps the file into process-local memory in the page tables.
    /* void *addr */ NULL,       // Start address (let the system choose)
            /* size_t length */ sb.st_size,  // Length of the mapping
            /* int prot */ PROT_READ,    // Memory protection (read-only)
            /* int flags */ MAP_PRIVATE, // Map the file privately (changes are not reflected in the file)
            /* int fd */ fd,             // File descriptor of the file to map
            // off_t offset  0         // Offset within the file (start at the beginning)

    address = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);

    // Error handle if MAP_FAILED return value
    if (address == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }


    //Write the contents of the newly mapped memory to stdout!
    if (write(STDOUT_FILENO,address,sb.st_size) != sb.st_size){
        perror("write");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);

}