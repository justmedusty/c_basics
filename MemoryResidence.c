//
// Created by dustyn on 4/16/24.
//

/*
 * This program demonstrates memory-residence information about pages in the virtual address range
 * and locks specific pages in memory using the mincore() and mlock() system calls.
 *
 * Locking memory pages is a useful tool as it allows you to manually prevent a page from being swapped to disk.
 * When a page is swapped to disk and subsequently accessed again, the CPU will respond with a page fault since this
 * page of memory does not physically exist in RAM. From here, the kernel will take over and it will load the page into physical memory
 * again from the disk. This is very time-consuming, and with any computing that is time-sensitive, you may want to lock
 * your important memory pages to prevent them from being swapped to disk.
 */

#include <unistd.h>
#include <stdlib.h>
#include "sys/mman.h"
#include "stdio.h"
#include "string.h"
#include "MichaelKerriskCode/get_num.h" // Assuming this is a custom header file for getting numbers

// Function to display memory-residence information
static void displayMincore(char *addr, size_t length) {

    unsigned char *vec;

    long pageSize, numPages, j;

    pageSize = sysconf(_SC_PAGESIZE); // Get the system's page size

    numPages = (length + pageSize - 1) / pageSize; // Calculate the number of pages

    vec = malloc(numPages); // Allocate memory to hold memory-residence information

    // Check for malloc failure
    if (vec == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Retrieve memory-residence information using mincore() system call
    if (mincore(addr, length, vec) == -1) {
        perror("mincore");
        exit(EXIT_FAILURE);
    }

    // Display memory-residence information
    for (j = 0; j < numPages; j++) {

        // Print the virtual address and corresponding memory-residence information
        if (j % 64 == 0) {

            printf("%s%10p : ", (j == 0) ? "" : "\n", addr + (j * pageSize));
            printf("%c", (vec[j] & 1) ? '*' : '.');
        }

    }
    printf("\n");

    // Free the memory allocated for memory-residence information
    free(vec);
}

// Main function
int memory_residence(int argc, char *argv[]) {

    char *addr;
    size_t len, lockLen;
    long pageSize, stepSize, j;

    // Check for correct number of command-line arguments
    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "Usage Error : mem num-pages lock-page-step lock-page-len\n");
        exit(EXIT_FAILURE);
    }

    // Get the system's page size
    pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }

    // Convert command-line arguments to integers and calculate lengths
    len = getInt(argv[1], GN_GT_0, "num-pages") * pageSize;
    stepSize = getInt(argv[2], GN_GT_0, "lock-page-step") * pageSize;
    lockLen = getInt(argv[1], GN_GT_0, "lock-page-len") * pageSize;

    // Allocate memory for the virtual address range
    addr = mmap(NULL, len, PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Print information about the allocated memory
    printf("Allocated %ld (%#lx) bytes starting at %p\n", (long) len, (unsigned long) len, addr);

    // Display memory-residence information before locking pages
    printf("Before mlock:\n");
    displayMincore(addr, len);

    // Lock pages in memory using mlock()
    for (j = 0; j + lockLen <= len; j += stepSize) {
        if (mlock(addr + j, lockLen) == -1) {
            perror("mlock");
            exit(EXIT_FAILURE);
        }
    }

    // Display memory-residence information after locking pages
    printf("afterMlock : \n");
    displayMincore(addr, len);

    return 0;
}