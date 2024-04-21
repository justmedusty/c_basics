//
// Created by dustyn on 4/21/24.
//

#define PORT "6969"

#include <netdb.h>
#include <string.h>
#include "sys/socket.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/stat.h"

#define BUFFSIZE 4096

/*
 * This is the client program for our file transfer client/server over TCP.I will not comment the regular socket init and connection systemc c since I already commented this extensively in other files.
 * I will just comment the necessary file transfer portion.
 *
 * I am assuming you understand the basics of socket programming before you read this.
 */

int main2(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: ftcp server_ip file_path");
    }

    int sock_fd;
    struct addrinfo hints, *pointer, *server;
    int return_value;
    char buffer[BUFFSIZE];
    int yes = 1;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((return_value = getaddrinfo(argv[1], PORT, &hints, &server)) != 0) {
        fprintf(stderr, "Error get server address info with error %s\n", gai_strerror(return_value));
        exit(EXIT_FAILURE);
    }

    for (pointer = server; pointer != NULL; pointer = pointer->ai_next) {
        if ((sock_fd = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1) {
            perror("Couldn't make socket and get file descriptor from kernel\n");
            continue;
        }


        if (connect(sock_fd, pointer->ai_addr, pointer->ai_addrlen) == -1) {
            close(sock_fd);
            perror("Connection failed");
            continue;
        }

        break;
    }

    if (pointer == NULL) {
        fprintf(stderr, "Error in creating socket\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server);


    /*
     * We are going to open the file at path specified by the 3rd arg to this program.
     * We will open with fopen using the flag "rb" which means read binary.
     * After which we will init a stat struct which we we will call stat() on to fill the struct
     * with information about that file with information from the i-node table. We will use this to veryify
     * the amount of bytes being sent.
     *
     * We will loop on fread filling our buffer until we reach EOF.
     *
     */
    FILE *file = fopen(argv[2], "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    printf("Sending file\n");



    size_t bytes_read;
    struct stat stat1;


    if(stat(argv[2],&stat1) == -1){
        perror("stat");
        exit(EXIT_FAILURE);
    }

    size_t byte_total = 0;

    /*
     * We will loop on fread until we reach end of file, we will read sizeof char * buffsize, since of char is always 1 byte so we will read BUFFSIZE bytes, chunking out
     * the file reading to keep things clean. As we read into the buffer we'll fire off binary data out to the server over the open connection we have on our socket.
     * Iterate byte total after each iteration.
     */
    while ((bytes_read = fread(buffer, sizeof(char), BUFFSIZE, file)) > 0) {
        if(byte_total == stat1.st_size){
            printf("Full file sent\n");
            break;
        }
        ssize_t bytes_sent = send(sock_fd, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
        byte_total += bytes_read;
    }

    /*
     * Here we will do a check on the size of the bytes sent, make sure we sent all of it
     */
    if(byte_total >= stat1.st_size) {
        printf("bytes sent : %lo bytes total in file %lo\n",byte_total,stat1.st_size);

    }
    if (ferror(file)) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    /*
     * That's it ! close the socket fd and the file!
     */

    close(sock_fd);


    fclose(file);

    exit(EXIT_SUCCESS);




}
