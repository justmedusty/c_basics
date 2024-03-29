//
// Created by dustyn on 3/27/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//We will define our port here
#define PORT "6969"
//And define the max bytes for a message , 256 sounds like a good number
#define MAX_BYTES 256
/*
//Same pointer function as the family, this will just spit out the memory address for either an IPv4 or an IPv6 address
void *get_in_addr (struct sockaddr *sockAddr){
    //If the family is of IF_INET(IPv4), cast it to an IPV4 struct with IPv4 address
    if (sockAddr->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sockAddr)->sin_addr);
    }
    //Else return memory address of IPv6 address cast to IPv6 struct
    return &(((struct sockaddr_in6*)sockAddr)->sin6_addr);
}

*/
//Our stream socket client , if you want to run this you'll need to change it to main
//argc is number of arguments, argv is array of arguments
int streamSocketClient(int argc, char* argv[]){
    //Our client socket file descriptor and byte total of the message
    int sockFd,byteTotal;
    //The buffer which be used for the message, with max size of MAX_BYTES defined above
    char buf[MAX_BYTES];
    //Our addr info structs, hints which we use to fill serverInfo and then assign to p which will give us our linked list of addresses we can walk to find what we are looking for
    struct addrinfo hints,*serverInfo,*p;
    //Return value of getaddrinfo
    int returnValue;
    //The string which will hold the address in presentation state, once we convert from network state with ntop() system call
    char string[INET6_ADDRSTRLEN];

    //You can also just uncomment this here if you want to run in IDE and not compile the executable, and run them both independently
    /*
    argc =2;
    argv[0] = "client";
    argv[1] = "0.0.0.0";
     */
    //Check that the program arguments come out to 2, client and hostname I left a snippet in the server code that will print out the server hostname for you should you not be sure
    if(argc!=2){
        fprintf(stderr,"usage: client hostname\n");
        exit(EXIT_FAILURE);
    }
    //Memory set hints pointed-to address to 0;
    memset(&hints,0,sizeof hints);
    //IPv4
    hints.ai_family = AF_INET;
    //Stream socket
    hints.ai_socktype = SOCK_STREAM;

    //We will check the return value here of getaddr info for getting the server information , if it succeeds it will assign the information we are looking for to the *serverInfo addrinfo struct! You can thank the
    //kernel for that, if error print error
    if((returnValue = getaddrinfo(argv[1], PORT,&hints,&serverInfo))!= 0){
        fprintf(stderr,"getaddrinfo error : %s\n", gai_strerror(returnValue));
    }

    //Here we will iterate through the linked list by assigning p to the addrinfo struct and walking through it, trying to create a socket and getting the
    //file descriptor from the kernel,
    for(p=serverInfo;p!=NULL;p=p->ai_next){
        //Here we will try to get the socket file dscriptor with the system call, socket, if error then iterate to the next entry in the list and start over
        if((sockFd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1){
            perror("Error creating client socket");
            continue;
        }
        //Attempt to connect to the server socket
        if(connect(sockFd,p->ai_addr,p->ai_addrlen) == -1){
            close(sockFd);
            perror("Error connecting client");
        }
        //break the loop
        break;
    }
    //If the pointed to entry in the list is NULL, something went wrong and we need to handle this by returning with an error code (non 0)
    if (p==NULL){
        fprintf(stderr,"Client failed to connect");
        return 2;
    }
    //Here we will get the presenation of the server address because we do not want binary, we want a char array. We do this with the inet_ntop(network to presentation) system call !
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr), string, sizeof string);
    //print out the string address of the socket we are connecting to
    printf("Client is connecting to %s\n",string);


    //We can free serverInfo since we are done with the memory now
    freeaddrinfo(serverInfo);
    //Here we attempt to receive a message through the socket from the server, and assign it to byteTotal, if error (-1 return value) handle accordingly
    if((byteTotal = recv(sockFd,buf,(MAX_BYTES -1),0))== -1){

        perror("Error receiving message from server");
        exit(EXIT_FAILURE);

    }

    //Here we will put a null terminator at the end of the message to signify that the message ends here
    buf[byteTotal] = '\0';
    //Print out the message here!
    printf("client received this message : %s\n",buf);
    //Close the socket file descriptor because we are done with it
    close(sockFd);
    //And exit on success code ! (0) You're a unix programmer now!
    return(EXIT_SUCCESS);
}
