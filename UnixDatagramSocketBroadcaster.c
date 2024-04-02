//
// Created by dustyn on 4/2/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//Our server port
#define SERVERPORT 6969

int udpBroadcaster(int argc, char *argv[]){

    //socket file descriptor which will send our message out
    int sockFd;

    //The client address , which will be for our broadcast message
    struct sockaddr_in clientAddress;

    //Hostentry hostent struct pointer
    struct hostent *hostEnt;

    //bytes sent
    int bytesSent;

    //Will be our affirmation for sock options
    int broadcast = 1;

    //Typical arg check and error handle appropriately, inform user of proper usage
    if(argc != 3){
        fprintf(stderr,"Usage : broadcaster hostname message\n");
        exit(EXIT_FAILURE);
    }

    //Get the hostname via gethostbyname from the second command line argument, error handle appropriately
    if((hostEnt= gethostbyname(argv[1])) == NULL){
        perror("gethostbyname() failed\n");
        exit(EXIT_FAILURE);
    }

    //Attempt to create our socket, error handle appropriately
    if((sockFd = socket(AF_INET,SOCK_DGRAM,0)) == -1){
        fprintf(stderr,"Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    //Here we will set our socket options, and make sure to use SO_BROADCAST which allows broadcasts, without this you will get a permission denied response!
    //Error handle appropriately
    if(setsockopt(sockFd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof broadcast) == -1){
        perror("Setting sock options with SO_BROADCAST\n");
        exit(EXIT_FAILURE);
    }
    //Here will assign some of the members of our clientAddress struct, importantly the port as well as the address,which will be the host address of the network
    clientAddress.sin_family = AF_INET;

    //We use htons here since it needs to be converted to network format to be valid on the wire
    clientAddress.sin_port = htons(SERVERPORT);
    clientAddress.sin_addr = *((struct in_addr *) hostEnt->h_addr );

    //We do not want to pad the size so put a null terminator across the entire memory space
    memset(clientAddress.sin_zero,'\0', sizeof clientAddress.sin_zero);

    //Here we attempt to sendto, which again is a udp function, and we will attempt our broadcast message to all devices on the network! Error handle appropriately
    if((bytesSent = sendto(sockFd,argv[2], strlen(argv[2]),0,(struct sockaddr *)&clientAddress,sizeof clientAddress)) == -1){
        perror("Error on sendto system call");
        exit(EXIT_FAILURE);
        }

    //Print the number of bytes send and the network to address function result of our client socket internet address
    printf("sent %d bytes to %s\n",bytesSent, inet_ntoa(clientAddress.sin_addr));

    //close the file descriptor since we are done
    close(sockFd);

    //bada bing bada boom
    exit(EXIT_SUCCESS);


}