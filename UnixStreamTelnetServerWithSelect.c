//
// Created by dustyn on 4/3/24.
//

#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define PORT "6969"
#define backlog 15
#define BUFFER_SIZE 1024

void *get_internet_address(struct sockaddr *addr) {
    if (addr->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) addr)->sin_addr);
    } else if (addr->sa_family == AF_INET6) {
        return &(((struct sockaddr_in6 *) addr)->sin6_addr);
    } else {
        fprintf(stderr, "Sockaddr was neither ipv4 or ipv6\n");
        exit(EXIT_FAILURE);
    }
}

int get_listener_socketFd() {
    int listenerFd;
    struct addrinfo hints, *serverInfo, *pointer;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int return_value;
    socklen_t socklen;
    int yes = 1;


    if ((return_value = getaddrinfo(NULL, PORT, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "Error occurred on getaddrinfo() call .. %s\n", gai_strerror(return_value));
        exit(EXIT_FAILURE);
    }
    if (serverInfo == NULL) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    for (pointer = serverInfo; pointer != NULL; pointer = pointer->ai_next) {

        if ((listenerFd = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) < 0) {
            continue;
        }
        if (setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
            strerror(errno);
            exit(EXIT_FAILURE);


        }
        if (bind(listenerFd, pointer->ai_addr, pointer->ai_addrlen) < 0) {
            close(listenerFd);
            continue;
        }

        break;

    }
    if (pointer == NULL) {
        perror("Error on bind\n");
        exit(EXIT_FAILURE);
    }


    freeaddrinfo(serverInfo);


    if (listen(listenerFd, backlog) != 0) {
        strerror(errno);
        exit(EXIT_FAILURE);
    }

    return listenerFd;


}


int main() {

    fd_set select_set, master_set;
    int fd_max;
    int listener;
    socklen_t socklen;
    struct sockaddr_storage client_address;
    char buffer[BUFFER_SIZE];
    struct timeval tv;
    int return_value;
    int newFd;
    int bytes_received;
    int bytes_sent;
    char clientIP[INET6_ADDRSTRLEN];


    listener = get_listener_socketFd();
    fd_max = listener;
    FD_ZERO(&select_set);
    FD_ZERO(&master_set);
    FD_SET(listener, &master_set);

    tv.tv_sec - 5;
    tv.tv_usec = 0;


   for(;;){
        select_set = master_set;
        if ((return_value = select(fd_max + 1, &select_set, NULL, NULL, NULL)) <= 0) {
            strerror(return_value);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i <= fd_max; i++) {
            if (FD_ISSET(i, &select_set)) {
                if (i == listener) {
                    socklen = sizeof client_address;
                    newFd = accept(listener, (struct sockaddr *) &client_address, &socklen);

                    if (newFd == -1) {
                        strerror(errno);
                    } else {
                        FD_SET(newFd, &master_set);
                        if (newFd > fd_max) {
                            fd_max = newFd;
                        }
                        printf("Server : New Connection from %s\n", inet_ntop(client_address.ss_family,
                                                                              get_internet_address(
                                                                                      (struct sockaddr *) &client_address),
                                                                              clientIP, INET6_ADDRSTRLEN));

                        strlcpy(buffer,"User joined chat\n",sizeof "User joined chat\n");
                        bytes_received = sizeof "User joined chat\n";
                        for (int j = 0; j < fd_max; j++) {
                            if (FD_ISSET(j, &master_set)) {
                                if (j != listener && j != i) {
                                    if (send(j, buffer, bytes_received, 0) == -1) {
                                        perror("send");

                                    }

                                }
                            }
                        }

                    }
                } else {
                    if ((bytes_received = recv(i, buffer, sizeof buffer, 0)) <= 0) {
                        if (bytes_received == 0) {
                            printf("Client socket %d disconnected\n", i);
                            strlcpy(buffer,"User left chat\n",sizeof "User left chat\n");
                            bytes_received = sizeof "User left chat\n";
                            for (int j = 0; j < fd_max; j++) {
                                if (FD_ISSET(j, &master_set)) {
                                    if (j != listener && j != i) {
                                        if (send(j, buffer, bytes_received, 0) == -1) {
                                            perror("send");

                                        }

                                    }
                                }
                            }
                            } else {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        for (int j = 0; j < fd_max; j++) {
                            if (FD_ISSET(j, &master_set)) {
                                if (j != listener && j!=i){
                                    if(send(j,buffer,bytes_received,0) == -1){
                                        perror("send");

                                    }

                                }
                            }

                        }
                    }
                }

            }
        }
    }

   return EXIT_SUCCESS;


}