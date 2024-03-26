//
// Created by dustyn on 3/26/24.
//
#include <sys/socket.h>
#include <netdb.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>


int main(){

    //We will request our internet protocol address information from the kernel, we will do this by
    //having both hints, and serverInfo for the results of our kernel call.
    int status ;
    //We will fill this struct to pass to getaddrinfo()
    struct addrinfo hints;
    //This will point to the results of asking the kernel for
    struct addrinfo *serverInfo;
    //Set memory to 0
    memset(&hints,0,sizeof(hints));
    //Address unspecified so can be either ipv4 OR ipv6
    hints.ai_family = AF_UNSPEC;
    //Socket type of stream which are tcp sockets. Datagram sockets are UDP sockets.
    hints.ai_socktype = SOCK_STREAM;
    //Passive meaning please fill in my ip for me
    hints.ai_flags = AI_PASSIVE;

    //We will run getaddr info with no hostname, port 6969, pass hints as input, and receiver serverInfo as output
    if((status = getaddrinfo(NULL,"6969",&hints,&serverInfo))!= 0){
        //If non-zero return value, error occurred and use gai_strerror(status) to print the status
        fprintf(stderr,"getaddrinfo error %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    //Iterate through the list by dereferencing the pointer to the next entry in the list
    for(int i = 0;serverInfo != NULL; i++) {
        //We can check the ip addresses available , we will dereference the serverInfo pointer to access the memory where cannon name is stored, and then do the same for dereferencing the ai addr field of serverInfo and again dereferencing to access the socket
        //address data stored in sa_data of the sockaddr struct as defined in socket.h header file.

        printf("This is entry %d in the list \n",i);

        //Is this an IPv6 Address?
        if (serverInfo->ai_family == AF_INET6){
            char ipaddr6[INET6_ADDRSTRLEN];
            //If so , convert this network representation to presentation which will be readable and not binary. We do this by declaring this is IPv6(AF_INET6), dereferencing the ai_addr sockaddr struct and then again dereferencing the socket address data field
            //we then assign this so the ipaddr6 chararray of size INET6_ADDRSTRLEN (the length of an ipv6 address
            inet_ntop(AF_INET6,&serverInfo->ai_addr->sa_data,ipaddr6,INET6_ADDRSTRLEN);
            printf("The IPv6 Address is %s\n",ipaddr6);
            //Is this an IPv4 address?
        }else if (serverInfo->ai_family == AF_INET){
            char ipaddr[INET_ADDRSTRLEN];
            //Again ,convert from network presentation to normal presentation, we do not want a binary address. Dereference the address information pointer that points to the socket address field of the sockaddr struct
            //Assign to ipaddr which is a char of size INET_ADDRSTRLEN ( the size of the an ipv4 address
            inet_ntop(AF_INET,&serverInfo->ai_addr->sa_data,ipaddr,INET_ADDRSTRLEN);
            printf("The IPv4 Address is %s\n",ipaddr);
        }
        //Iterate through the linked list by referencing our result struct and setting it to the pointer of the next entry in the list
        serverInfo = serverInfo->ai_next;


    }
    //Free the linked list when we are done
    freeaddrinfo(serverInfo);
}
