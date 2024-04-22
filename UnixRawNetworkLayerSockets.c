
//
// Created by dustyn on 4/22/24.
//

#include <netdb.h>
#include "sys/socket.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "poll.h"

#define PORT "6969"
#define BACKLOG 15
#define PACKETSIZE 4096
/*
 * Raw sockets are a powerful feature in UNIX. Raw sockets remove the kernels implementation
 * of the network layer protocols from the equation, and what you end up with is your own transport
 * level playground. Because we will be handling the entire transport layer, we will use sendmsg and recvmsg instead of our usual send/recv.
 * sendmsg and recvmsg alongside a msgheader structure. Because we will be working with the network layer directly here,
 * we will need to pass message metadata in headers such as its size and maybe some kind of packet identifier for acknowledgement
 * from the receiver. We will go through this together.
 *
 * The network layers purpose is getting a packet from point A to point B, ensuring there wasn't any data corruption or
 * the like will be up to us. We can implement some basic checks and balances.
 *
 * All the typical server code, again, I will not comment because I have commented all of it in my other Unix networking files.
 *
 *
 * This is more advanced, so I am assuming you know what you are looking at.
*/

void* get_internet_addresses(struct sockaddr *sock_address){

    if(sock_address->sa_family == AF_INET){
        return &(((struct sockaddr_in *)sock_address)->sin_addr);
    } else{
        return &(((struct sockaddr_in6 *)sock_address)->sin6_addr);
    }
}

int get_listener(){

    int listener;
    struct addrinfo hints,*server,*pointer;

    hints.ai_socktype = SOCK_RAW;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    int return_value;

    if((return_value = getaddrinfo(NULL,PORT,&hints,&server)) < 0){
        gai_strerror(return_value);
        exit(EXIT_FAILURE);
    }

    for(pointer = server;pointer != NULL; pointer = pointer->ai_next){

        if((listener = socket(pointer->ai_family,pointer->ai_socktype,pointer->ai_protocol) < 0)){
            perror("socket");
            continue;
        }

        if(bind(listener, pointer->ai_addr,pointer->ai_addrlen) < 0) {
            perror("bind");
            continue;
        }

        break;

    }

    if(pointer == NULL){
        perror("bind");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server);

    if(listen(listener,BACKLOG) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return listener;

}


int main(){

    int listener;
    char buffer[PACKETSIZE];
    int new_fd;
    struct sockaddr_storage client_address;
    struct pollfd poll_listen;

    listener = get_listener();

    if (listener <= 0){
        exit(EXIT_FAILURE);
    }



}