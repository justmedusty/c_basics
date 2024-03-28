//
// Created by dustyn on 3/28/24.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

//Our port , as always
#define PORT "6969"

//Our UDP client function, takes argc which is number of args, and argv which is array of arguments passed
int udpClient(int argc, char *argv[]) {
    //Our server socket file descriptor
    int sockFd;
    //Again our 3 addressinfo entries, with the serverInfo pointer for holding results, hints for giving initial info to the kernel, and pointer to iterate through the linked list afterwards
    struct addrinfo hints, *serverInfo, *pointer;
    //Return value of getaddrinfo systemcall
    int returnValue;
    //byte total of message sent to the server
    int byteTotal;

    //You can manually set argc and argv[0]..argv[2] here if you wish to do this from an IDE

    //This is just a check to make sure the command was used properly with all 3 args (./yourcompiledexecutable udp hostname hello)
    if (argc != 3) {
        fprintf(stderr, "usage: udp hostname message\n");
        exit(EXIT_FAILURE);
    }


    //Utilize memset in order to make sure that hints has its memory set to 0 all the way across
    memset(&hints, 0, sizeof hints);
    //Family is AF_INET which again is we don't care if it's IPv6 or IPv4 we'll take either
    hints.ai_family = AF_INET;
    //This is a UDP client and server so we are sure as hell gonna go with data gram socket protocol
    hints.ai_socktype = SOCK_DGRAM;
    //Make sure to use this so that the kernel gives you the addr data
    hints.ai_flags = AI_PASSIVE;

    //Attempt to fill the linked list pointer with the address of a list returned by the kernel, error handle appropriately
    if ((returnValue = getaddrinfo(argv[1], PORT, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "Error get server address info with error %s\n", gai_strerror(returnValue));
        exit(EXIT_FAILURE);
    }
    //Iterate through the linked list and attempt to create a socket and assign the socket file descriptor based on the kernels response, handle error return value appropriately
    for (pointer = serverInfo; pointer != NULL; pointer = pointer->ai_next) {

        if ((sockFd = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1) {
            perror("Couldnt make socket and get file descriptor from kernel\n");
            //Continue to the next iteration and try again
            continue;
        }
        break;

    }
    //Socket creation failed if the pointer is pointing to a null entry in the list
    if (pointer == NULL) {
        fprintf(stderr, "Error in creating socket\n");
        exit(EXIT_FAILURE);
    }
    //Here we will use the sendto systemcall to send the message passed as argument 3 to the program , to the address that the kernel gave us earlier in that linked list of struct addrinfo
    //Handle error return value appropriately
    //What is nice about udp again, we do not need a connect() system call, we can just fucking fire it off!. It can only be received if there is a server checking with recvfrom.
    //Handle errors accordingly
    //You can have this to take input as well or really do whatever, loop it and then have this send messages to the socket based on some external input
    if ((byteTotal = sendto(sockFd, argv[2], strlen(argv[2]), 0, pointer->ai_addr, pointer->ai_addrlen)) == -1) {
        fprintf(stderr, "Error sending message in sendto system call\n");
        exit(EXIT_FAILURE);
    }
    //We're done with serverInfo now so let's clear that up
    freeaddrinfo(serverInfo);
    //Print the message size as well the message and the server hostname
    printf("UDP Client sent %d bytes to server , with message %s to server %s\n", byteTotal, argv[2], argv[1]);
    //exit with success code !
    exit(EXIT_SUCCESS);

    //And that's it! A udp client! Very barebones but these are the building blocks for proper clients and server using datagram sockets in unix!


}