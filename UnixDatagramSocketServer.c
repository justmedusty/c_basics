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


#define PORT "6969"
#define MAX_BYTES 256
/*
//Same function in the other files, just will get us our address whether it is IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) {
    //If the family is of IF_INET(IPv4), cast it to an IPV4 struct with IPv4 address
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }
    //Else return memory address of IPv6 address cast to IPv6 struct
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}
*/
//Our datagram server main function
int unixDatagramSocketServer(void) {
    //Socket file descriptor which will be assigned by the kernel and how we will identify the socket
    int sockFd;
    //3 addr info hints which we will fill in a little information in hints, then fill serverInfo with the address to the results, and then use pointer to iterate through the linked list
    struct addrinfo hints, *serverInfo, *pointer;
    //byte total of the message received
    int byteTotal;
    //This will store the clientAddress, you use sockaddr_storage since we do not yet know if it will be IPv4 or IPv6
    struct sockaddr_storage clientAddress;
    //The message buffer, with a maximum size of MAX_BYTES defined above
    char buffer[MAX_BYTES];
    //This will hold the addressLength of which we will need in the recvfrom call
    socklen_t addressLength;
    //Return value of the getaddrinfo system call
    int returnValue;
    //This will hold the string representation of the senders IP addr, of size INET6 because it can hold either IPv4 or IPv6
    char string[INET6_ADDRSTRLEN];

    //Make sure we clear all of the memory just in case, set it all to 0
    memset(&hints, 0, sizeof hints);
    //AF_INET aka either IPv4 or IPv6 whatever it happens to be
    hints.ai_family = AF_INET;
    //Datagram socket type, which is for UDP (User datagram protocol) which means that there is no guarantee that your packet will make it ,but you do not need to use the connect() system call and can just use sendto and recvfrom which will just fire off
    //packets whether they make it or not only god knows
    hints.ai_socktype = SOCK_DGRAM;
    //Passive flag meaning fill this address info in for me please oh mighty kernel, for I am lazy
    hints.ai_flags = AI_PASSIVE;

    //Here we will call getaddrinfo with no name, the port as defined at the top of the file, take the information we inserted into the hints struct, and fill the server info struct with the linked list of results. Error handle on bad return value
    if ((returnValue = getaddrinfo("0.0.0.0", PORT, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "Error on getaddrinfo call : %s\n", gai_strerror(returnValue));
        exit(EXIT_FAILURE);
    }
    //Here we will iterate through the returns linked list which is pointed to by serverInfo , which we will assign to our pointer to do some pointer list iteration
    for (pointer = serverInfo; pointer != NULL; pointer = pointer->ai_next) {

        //Attempt to create the socket and return the socket file descriptor from which the kernel will be able to identify it for us, if we can't iterate the list
        if ((sockFd = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1) {
            perror("Error creating socket and assigning to socket file descriptor\n");
            continue;
        }
        //Attempt to bind the address in this list entry to the socket file descriptor we just created, if not iterate to the next entry in the list
        if (bind(sockFd, pointer->ai_addr, pointer->ai_addrlen) == -1) {
            close(sockFd);
            perror("Error binding socket to address info\n");
            continue;
        }
        //Here will we just get and print the server address
        struct sockaddr_storage serverAddress;
        serverAddress.ss_family = pointer->ai_family;
        char hostname[MAX_BYTES];
        inet_ntop(serverAddress.ss_family, get_in_addr((struct sockaddr *) &serverAddress), hostname, sizeof hostname);
        printf("hostname is %s\n", hostname);

        //If both succeeded, break
        break;

    }
    //If the pointer to the list entry is null, the bind did not work properly and we need to exit the program
    if (pointer == NULL) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    //We can free up serverInfo since we are done with it
    freeaddrinfo(serverInfo);
    //Waiting for a recvfrom
    printf("Listener is awaiting a recvfrom call...\n");

    //Assigning the size dynamically now that it is known
    addressLength = sizeof clientAddress;

    //So , here is the differentiating , interesting part about datagram sockets. Notice that unlike in the stream socket server where we had to connect, there is no connection here.
    //We can just recvfrom and if we know the address, we can receive packets to our socket! On the flip side, you can use sendto instead of send, which just fires a packet off without any
    //connection before hand. If they are not watching the port with recvfrom then they will not catch the packet though. Standard error handling of course.
    if ((byteTotal = recvfrom(sockFd, buffer, (MAX_BYTES - 1), 0, ((struct sockaddr *) &clientAddress),
                              &addressLength)) == -1) {
        perror("Error on recv from");
        exit(EXIT_FAILURE);
    }
    //Here we will get the client address using the network to presentation system call, since we do not want binary, We will make a call to our pointer function get_in_address with the client address to get the proper (either IPv4 or IPv6)
    //address
    inet_ntop(clientAddress.ss_family, get_in_addr((struct sockaddr *) &clientAddress), string, sizeof string);
    //And print it to the screen alongside the size of the message in bytes, as well as the size of the message and the address of the sender as assigned by the system call above
    printf("Listener packet is %lu bytes long wit content %s, received from %s\n", sizeof byteTotal, buffer, string);
    //SET YOUR NULL TERMINATOR
    buffer[byteTotal] = '\0';



    //Close the server socket file descriptor, no looping here just 1 and done
    close(sockFd);
    //Exit on success code
    exit(EXIT_SUCCESS);
}