//
// Created by dustyn on 3/27/24.
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

//The port from which the client will connect to
#define PORT "6969"
//The backlog will define how many connections can be in the queue at a time
#define backlog 5

//This function is for handling the SIGCHLD signal that the parent process receives from the kernel in unix systems when a child process has been terminated
void signalChild_Handler(int socket) {

    //in case it gets overwritten by the waitpid() call below
    int saved_error = errno;
    //This line is important and the reason why it is important is the way unix handles processes
    //When a child process terminates the kernel will send a SIGCHLD signal to the parent to let it know that
    // a child process has temrinated, however, in unix, when a process is terminated it will remain as what is known as
    //a zombie process. By making the waitpid call here with the no hangup option, we will be able to clear out zombie processes
    //as they come , which is vital for keeping your sysadmin happy (lol) and more importantly not keeping your
    //process tree littered with zombie processes
    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_error;
}

//Pointer function to get the socket address whether it is an IPv4 or IPv6 addr
void *get_in_addr(struct sockaddr *sockAddr) {
    //If this is of family AF_INET (IPv4) we want to grab the sin_addr of this socket
    if (sockAddr->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sockAddr)->sin_addr);
    }
    //Else we want to grab the sin6_addr which is the IPv6 addr
    return &(((struct sockaddr_in6 *) sockAddr)->sin6_addr);
}


int server(void) {
    //Our server socket file descriptor with our client file descriptor which will be grabbed at runtime
    int sockfd, newFd;
    //These are our addrinfo structs, hints which will be used to pass to getaddrinfo which will then fill *serverInfo with the linked list we are looking for, and a pointer to the results
    struct addrinfo hints, *serverInfo, *p;
    //We use sockaddr_stoage here since it is protocol agnostic and can be cast to either IPv4 or IPv6 later
    struct sockaddr_storage clientAddress;
    //The size of the socket represented with the socklen_t type which is at least 32 bits
    socklen_t sin_size;
    //This signal action struct will be used later to handle zombie processes once children are terminated
    struct sigaction sa;
    //This is needed for setting socket options , the reuse address option flag which we will past this off to
    int yes = 1;
    //This is a string of length INET6_ADDRSTRLEN
    char s[INET6_ADDRSTRLEN];
    //This will be used to hold the return value of our initial getaddrinfo() system call
    int returnVal;
    //Variable for storing the hostname
    static char hostname[256];

    //Memset hints memory address to be all 0
    memset(&hints, 0, sizeof hints);
    //We dont care about IPv4 or IPv6 hence UNSPEC , we will find this out later
    hints.ai_family = AF_UNSPEC;
    //Stream sockets for TCP protocol
    hints.ai_socktype = SOCK_STREAM;
    //AI_PASSIVE flag to indicate we are lazy and want the kernel to fill in the ip info for us
    hints.ai_flags = AI_PASSIVE;

    //This will utilize the getaddrinfo system call to get the linked list of results from the kernel, and print to stderr if there is an issue
    if ((returnVal = getaddrinfo(NULL, PORT, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(returnVal));
        return EXIT_FAILURE;
    }
    //This will assign our pointer p which we declared above, run until it's null, and increment the list by dereferencing the ai_next field of the struct
    for (p = serverInfo; p != NULL; p = p->ai_next) {

        //Here we will get the server socket file descriptor by dereferencing the 3 fields , family,socket type, and protocol and passing them to the socket system call
        //We will check for a -1 value which of course means that there was an error and print to stderr
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Server Error getting socket file descriptor");
            //We can continue , rather than exit, because we need to iterate the whole list and exiting would mean if the first entry in the linked list is not what we are looking for, terminate the entire program right now
            continue;
        }
        //Here we will set our socket options, we will set the reuse address option, and if error (-1 return val) handle accordingly
        //Using the REUSEADDR flag can help bounce back from a crash since the server can use the same address which will speed up that process
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("Error setting socket options");
            exit(EXIT_FAILURE);
        }
        //Now we will attempt to bind the socket file descriptor to the address returned by getaddrinfo() earlier , if -1 return val handle error accordingly
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("Error binding socket");
            continue;
        }
        //Here we will print out the server IP by using inet_ntop (network -> presentation), with AF_INET for either ipv4 or v6, the ip address in the current entry in the linked list as pointed to by p* , assign it to ipaddr below
        char ipaddr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &p->ai_addr->sa_data, ipaddr, INET_ADDRSTRLEN);
        printf("Server IP bound to %s\n", ipaddr);

        getnameinfo(p->ai_addr, p->ai_addrlen, hostname, sizeof(hostname), NULL, 0, 0);
        printf("%s\n", hostname);

        break;
    }
    //We do not need serverInfo anymore since we have our p struct, we can free this memory now
    freeaddrinfo(serverInfo);
    //If p struct is null, the server did not bind properly, and we need to exit
    if (p == NULL) {
        fprintf(stderr, "Server failed to bind");
        exit(EXIT_FAILURE);
    }
    //Now we will listen with our socket file descriptor, and we will pass the backlog which we passed in the top of this file to indicate that max amount of connections that can sit in the queue
    if (listen(sockfd, backlog) == -1) {
        fprintf(stderr, "Error on listen");
        exit(EXIT_FAILURE);
    }
    //We will assign our signal action handler to our function defined above, signalChild_Handler, this is simply for dealing with zombie
    //processes since they can really add up in unix systems
    sa.sa_handler = signalChild_Handler;
    //We will empty the socket address mask
    sigemptyset(&sa.sa_mask);
    //And set the socket address flag to SA_RESTART
    sa.sa_flags = SA_RESTART;
    //We will check for the SIGCHLD signal which is sent to the parent when the child is terminated , this is when we will need to go in and eliminate the remaining zombie process
    //Zombie processes are very important to remember in unix, over time they will be a drain on your server and also leak resources. In Unix systems , processes when terminated stay alive in a zombie state, which then need to be
    //taken care of
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        fprintf(stderr, "Error on signal action");
        exit(EXIT_FAILURE);
    }

    printf("Server is waiting for connections......\n");

    //Our main connection loop
    while (1) {
        //socket inet is the size of the client address
        sin_size = sizeof(clientAddress);
        //We will assign the new file descriptors to the result of our accept() system call which will accept the connection, handle error accordinngly
        newFd = accept(sockfd, (struct sockaddr *) &clientAddress, &sin_size);
        if (newFd == -1) {
            fprintf(stderr, "Error accepting new connection");
            //Continue not exit because 1 connection failing is not justification to shut it all down, this happens and we will accept it.
            continue;
        }
        //This will get the presentation (string) representation of the network address (it is binary if we dont use this system call) and it will be assigned to string s declared above
        inet_ntop(clientAddress.ss_family, get_in_addr((struct sockaddr *) &clientAddress), s, sizeof(s));

        printf("Server got connection from %s \n", s);
        //Fork a new process for each incoming connection
        if (!fork()) {
            //Close child process file descriptor
            close(sockfd);
            //Attempt to send "Testing 123" to the connected client file descriptor, handle error accordingly
            if (send(newFd, "Testing 123", sizeof("Testing 123"), 0) == -1) {
                fprintf(stderr, "Error sending message to file descriptor %d\n", newFd);
                close(newFd);
                exit(EXIT_SUCCESS);

            }
            //Close the client file descriptor in this child process
            close(newFd);
        }

        close(sockfd);
        //Exit on success code (0) !
        return EXIT_SUCCESS;
    }
    //And just like that, you're a unix network programmer !
    //We will get into writing a stream socket client next ! and then we can explore datagram (udp) sockets  in unix !
}

