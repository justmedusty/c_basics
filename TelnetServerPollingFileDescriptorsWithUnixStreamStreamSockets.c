//
// Created by dustyn on 3/31/24.
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
#include <poll.h>

//The port from which the client will connect to
#define PORT "6969"
//The backlog will define how many connections can be in the queue at a time
#define backlog 10


/*
 * Now polling is important, it is one of 3 ways to handle incoming messages on your sockets
 * by default when iterating through sockets, it will block indefinitely until you get something
 * which is obviously not ideal at all. Polling allows you to wait for an event that lets you know
 * there is something ready to be read, or wrote depending on your needs. Here we will go through a
 * basic telnet server which users can telnet to and each message they send is broadcast to all other members
 * in the chat currently. we will use poll() to handle the blocking problem. You can also use select() or epoll().
 * Epoll is linux specific whereas poll and select are standards across unix-like operating systems
 *
 */






//Pointer function to get the socket address whether it is an IPv4 or IPv6 addr
void *get_in_addr(struct sockaddr *sockAddr) {

    //If this is of family AF_INET (IPv4) we want to grab the sin_addr of this socket (and cast it to the
    if (sockAddr->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sockAddr)->sin_addr);
    }

    //Else we want to grab the sin6_addr which is the IPv6 addr
    return &(((struct sockaddr_in6 *) sockAddr)->sin6_addr);
}

//This function will be used to get our listener socket which will listen for incoming connections on client sockets
int get_listener_socket(void) {

    //This will hold the listener socket file descriptor
    int listener;

    //This will hold the yes value for when we set socket options below
    int yes = 1;

    //This will hold the return value from the getaddrinfo call
    int returnValue;

    //This addrinfo struct , we will apply some values to below and make the system call getaddr info which the kernel will then fill addressInfo struct with all of the available addresses
    //we can bind to this socket, pointer will use for iteration through the result set
    struct addrinfo hints, *addressInfo, *pointer;

    //Make sure the memory is cleared to 0 across the board so we have a blank slate to work with
    memset(&hints, 0, sizeof hints);

    //As usual, our hints we will set family to AF_UNSPEC which just indicates that we don't care about ipv6 or ipv4
    hints.ai_family = AF_UNSPEC;

    //This address info passive flag just means we are asking the kernel to fill in address info for us, for we are lazy!
    hints.ai_flags = AI_PASSIVE;

    //Stream socket, since TCP makes sense for a telnet chat server
    hints.ai_socktype = SOCK_STREAM;

    //Here we will make the getaddrinfo system call which will fill addressInfo with our result set !, handle errors accordingly. We can use the gai_strerror (get address info string error) function to get the string representation of the int error code ! very helpful
    if ((returnValue = getaddrinfo(NULL, PORT, &hints, &addressInfo) != 0)) {
        fprintf(stderr, "select server %s \n", gai_strerror(returnValue));
        exit(EXIT_FAILURE);

    }

    //Walk through the result set pointed to by our result set and try to create the listener socket with each given entry in the list, keep going until we get a winner
    for (pointer = addressInfo; pointer != NULL; pointer = pointer->ai_next) {
        listener = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol);
        if (listener < 0) {
            continue;
        }
        //we will set sock option flag SO_REUSEADDR so that we do not get any "address in use" complaints!
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        //If all has succeeded so far, we will attempt to bind our new socket to the corresponding address as specified in the result set.
        if (bind(listener, pointer->ai_addr, pointer->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        // We are handling errors appropriately along the way of course. Lastly , we will make a call to inet_ntop() to get our
        //address in string form as opposed to binary and then print it to the screen.
        char serverIp[INET6_ADDRSTRLEN];
       inet_ntop(pointer->ai_addr->sa_family, get_in_addr((struct sockaddr *) &pointer->ai_addr), serverIp, INET6_ADDRSTRLEN);
        printf("Server IP is %s\n",serverIp);
        break;
    }

    //Free the result set since we created and bound a socket!

    freeaddrinfo(addressInfo);

    //Check that the bind system call worked properly , exit on error code if not
    if (pointer == NULL) {
        fprintf(stderr, "Error occurred during bind process");
        close(EXIT_FAILURE);
    }

    //Here we will call the listener system call which will listen for connections on the given socket , and pass the backlog as our backlog defined above, this is the max queue length for connections.
    //As always, handle errors accordingly
    if (listen(listener, backlog) != 0) {
        fprintf(stderr, "Error on call to listen() with fd %d", listener);
        exit(EXIT_FAILURE);
    }

    //Return the file descriptor of our new listener socket!
    return listener;

}

//This will be a function for adding a new file descriptor to our set to be polled, dynamically allocate memory as needed using realloc to reallocate new memory when we run out of space
void add_to_pfds(struct pollfd *pollFileDescriptors[], int newFd, int *fd_count, int *fd_size) {
    //Double the size if we have run out of space
    if (*fd_count == *fd_size) {
        *fd_size *= 2;
        //Reallocate the new size (double what it previously was)
        *pollFileDescriptors = realloc(*pollFileDescriptors, sizeof(**pollFileDescriptors) * (*fd_size));
    }

    //Add our new file descriptor in this place
    (*pollFileDescriptors)[*fd_count].fd = newFd;
    (*pollFileDescriptors)[*fd_count].events = POLLIN;
    //Iterate the count so that we will be able to iterate over this file descriptor later
    (*fd_count) ++;

}

//We'll delete a poll by moving it to the end of the array and reducing the size by 1, you could also just set the fd to a negative value and it would be ignored
void del_pfds(struct pollfd pollFileDescriptors[], int i, int *fd_count) {
    pollFileDescriptors[i] = pollFileDescriptors[*fd_count - 1];
    (*fd_count)--;
}

//Our main function where our telnet server loop will happen
int telnetServerWithPolling(void) {

    //Our listener file descriptor which of which the listener socket will listen for connections for us
    int listener;
    //This newFd will be assigned dynamically as each POLLIN event happens
    int newFd;
    //This will also be assigned dynamically as each POLLIN event happens with a new telnet connection
    struct sockaddr_storage clientAddress;
    //This socklength type will store the length of the address
    socklen_t addrlen;

    //The buffer which will store the messages that are being
    char buffer[1024];
    //This will store our client address, we allocate size of INET6_ADDRSTRLEN because it is big enough to hold either IPv4 or IPv6
    char clientIP[INET6_ADDRSTRLEN];
    //Our count will start with 0 since we don't have any polling file descriptors yet
    int fd_count = 0;
    //Our initial size will be for 5 polling file descriptors
    int fd_size = 5;
    //Now we instantiate and allocate our array dynamically of size specified above (5 pollfds)
    struct pollfd *pollFileDescriptors = malloc(sizeof(*pollFileDescriptors) * fd_size);

    //We will assign the listener fd the result of our get_listener_socket() which we wrote above, this will spit out a listener for us
    listener = get_listener_socket();

    //Error handle to check if listener socket was created and bound properly
    if (listener == -1) {
        fprintf(stderr, "Error creating listener during call to get_listener_socket() \n");
        exit(EXIT_FAILURE);
    }

    //No we add it to our polling file descriptor array
    pollFileDescriptors[0].fd = listener;

    //We will assign event POLLIN which means we want to know when there is an incoming connection on this socket
    pollFileDescriptors[0].events = POLLIN;

    //Set our fd_count to 1 since we have just added our listener
    fd_count = 1;

    //Here we will start an infinite loop, for(;;) is shorthand for an infinite loop in C
    for (;;) {
        //We will make the poll system call, this is where we will actually start the polling for return events, timeout-1 to indicate that we do not want a timeout, go forever !
        int poll_count = poll(pollFileDescriptors, fd_size, -1);

        //Standard error checking, if return value is -1 it means an error happened
        if (poll_count == -1) {
            perror("Poll sys call failed");
            exit(EXIT_FAILURE);
        }

        //We will start our first loop on all polling file descriptors in the array
        for (int i = 0; i < fd_count; i++) {

            //If the return event is POLLIN...
            if (pollFileDescriptors[i].revents & POLLIN) {

                //We will check if it is the listener, POLLIN from the listener means that someone is ready to connect to our telnet server!
                if (pollFileDescriptors[i].fd == listener) {

                    //we will assign addr len to the length of clientAddress so there is enough space
                    addrlen = sizeof clientAddress;
                    //We will accept the connection that the listener is ready to give us, and we will feed it the address of clientAddr and addr len in order to
                    //let it fill in these values with the values from the client in  this new connection
                    newFd = accept(listener, (struct sockaddr *) &clientAddress, &addrlen);

                    //Standard error checking, if newFd is -1 , houston we have a problem
                    if (newFd == -1) {
                        perror("accept() system call");
                    } else {
                        //Otherwise add to our polling file descriptors list with a call to our add_to_pfds call above,which will add it to the list , dynamically handle size, and increment the count for us!
                        add_to_pfds(&pollFileDescriptors, newFd, &fd_count, &fd_size);

                        //Print out the new connection to the console with the ip address converted from network (binary) to presentation (string) via the inet_ntop() system call
                        printf("Poll server : new connection from %s  on socket %d\n",

                               inet_ntop(clientAddress.ss_family,
                                         get_in_addr((struct sockaddr *) &clientAddress), clientIP, INET6_ADDRSTRLEN),
                               newFd);

                    }

                    //Else it is NOT the listener, and it is a client waiting to send a message to the telnet server
                } else {

                    //Assign message bytes the return value of our recv systemcall, recv from this element in the pollfd array, the message will be fed into our 1024 bytes buffer that we created above, and no flags...
                    //It is important to note that if the buffer supplied is not big enough the message will be truncated, in which case the size will not match what is in the buffer ! make sure your buffer is large enough to handle whatever
                    //you are expecting since it will be cut off
                    int messageBytes = recv(pollFileDescriptors[i].fd, buffer, sizeof buffer, 0);

                    //We will assign the senderFd to this senders fd so that when we send our broadcast message we can skip over them, no sense sending their message back to them, it's for other members for the telnet chat
                    int senderFd = pollFileDescriptors[i].fd;

                    //Here we will check if is 0 or negative
                    if (messageBytes <= 0) {

                        //If recv returns 0 it means that the connection has been closed by the client, so we will print out an appropriate message
                        if (messageBytes == 0) {
                            printf("Connection closed on socket %d\n", senderFd);

                            //Else it means an error occured so print and error message
                        } else {
                            perror("recv from socket");
                        }

                        //Now we will ask the kernel to close this file descriptor to close the socket
                        close(pollFileDescriptors[i].fd);

                        //We will then make a call to del_pfds which will just move this element to the end of the array and then decrement the count which will just cast it into the void!
                        del_pfds(pollFileDescriptors,i,&fd_count);

                        //Else this means that it is time to broadcast this message to all clients connected to our telnet server!
                        //aside from the sender of course
                    } else {

                        //Nested loop for handling the broadcast message
                        for (int j = 0; j < fd_count; j++) {
                            //For each pollfd in the array ,aside from the sender and the listener, we will send the contents of the last message! broadcast it to everybody!
                            int destinationFd = pollFileDescriptors[j].fd;
                            if (destinationFd != listener && destinationFd != senderFd) {

                                //Send it !, error check of course
                                if (send(destinationFd, buffer, messageBytes, 0) == -1){
                                    perror("error on send to dest fd");
                                }

                            }


                        }
                    }


                }
            }

        }


    }


    //Unreachable code here but we don't give a fuck we'll write it anyway, we're freaks
    return EXIT_SUCCESS;

}