//
// Created by dustyn on 3/30/24.
//


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
//Where our socket FD will lie
#define SOCKET_PATH "/tmp/my_unix_socket"
//Here we will define our max queue length for connections waiting ! 10 sounds okay to me!
#define MAX_QUEUE_LEN 10

//Unix sockets are an important tool in the unix programming environment, they are one of many solutions to the IPC
//question...
//How can different processes communicate with each other on the system? Unix Sockets of course! These, from the programmers perspective,
//Are about the same as network sockets HOWEVER they only operate within the kernel!
int unixSockets() {

    //client and server file descriptors which will be assigned by the kernel when we're ready
    int server_fd, client_fd;

    //Unix socket address struct for both server and client
    struct sockaddr_un server_addr, client_addr;

    //This will store important data about the client socket
    socklen_t client_len;

    // Create a Unix domain socket, notice AF_UNIX (not AF_INET, STREAM (tcp) socket, no protocol passed handle this for us,
    //And do your standard error handling
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure with all 0s, we'll set the family of the server addr to AF_UNIX since
    //This is a unix socket,and copy  the path given in our temp file path above into the sun_path field
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family= AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind the socket to a file path and error handle appropriately, cast server_addr to struct sockaddr pointer
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections, with a max queue len as defined above! do standard error checking as listen
    //returns -1 on error
    if (listen(server_fd, MAX_QUEUE_LEN) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening...\n");


    //here we have an infinite loop, which will accept client connections using the accept() system call, and handle error return value
    //appropriately
    while (1) {

        // Accept incoming connections
        client_len = sizeof(client_addr);
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len)) == -1) {
            perror("accept");
            continue;
        }

        //If all is good, client is connected
        printf("Client connected.\n");

        // Handle client communication (send/receive data)
        //This is where you would use send and recv system calls to send and receive data between the client socket and server socket

        // Close client socket
        close(client_fd);
        printf("Client disconnected.\n");
    }

    // Close server socket
    close(server_fd);

    // Remove socket file from the file system
    //You might notice, dealing with the file system is unique to unix sockets, this is again because unix sockets
    //are primarily for IPC, inter-process communication and are handled by the kernel internally!
    unlink(SOCKET_PATH);

    return EXIT_SUCCESS;
}

