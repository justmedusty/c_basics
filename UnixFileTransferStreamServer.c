//
// Created by dustyn on 4/20/24.
//
#include <stdio.h>
#include "sys/socket.h"
#include "netdb.h"
#include "unistd.h"
#include "stdlib.h"
#include "poll.h"
#include "string.h"
#include "arpa/inet.h"

#define PORT "6969"
#define BACKLOG 15
#define MAX_PATH_SIZE 100
#define BUFFSIZE 4096
#define DOWNLOAD_FOLDER "/home/dustyn/Downloads"
/*
 * I am not going to comment any of the code that is very similar to other code I have used so far in other files since this is focusing on file transfer. If you are confused about any uncommented code here, go look at some of my other files to read
 * what each line and system call is doing. I am just going to write it out and not comment until we are ready for some file transfer
 *
 * Go look at my other unix socket programming files for intro stuff. I am assuming you know what you are looking at here.
 */
void* get_internet_address(struct sockaddr *sockad){

    if(sockad->sa_family == AF_INET){
        return &(((struct sockaddr_in *) sockad)->sin_addr);
    }
    else {
        return &((struct sockaddr_in6 *) sockad)->sin6_addr;
    }

}
int get_listener_socket(void){

    struct addrinfo hints, *results, *pointer;
    int listener_fd;
    int addr_ret;
    memset(&hints,0,sizeof hints);
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    int yes = 1;

    addr_ret = getaddrinfo(NULL,PORT,&hints,&results);

    if(addr_ret < 0){
        gai_strerror(addr_ret);
        exit(EXIT_FAILURE);
    }

    for(pointer = results;pointer!= NULL; pointer = pointer->ai_next ){

        listener_fd = socket(pointer->ai_family,pointer->ai_socktype,pointer->ai_protocol);
        if(listener_fd < 0){
            perror("socket");
            continue;
        }

        setsockopt(listener_fd,SOL_SOCKET,SO_REUSEADDR, &yes ,sizeof (int));

        if (bind(listener_fd, pointer->ai_addr,pointer->ai_addrlen) < 0){
            perror("bind");
            close(listener_fd);
            continue;
        }

        break;
    }

    if (pointer == NULL) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if(listen(listener_fd,BACKLOG) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    char serverIp[INET6_ADDRSTRLEN];
    inet_ntop(pointer->ai_addr->sa_family, get_internet_address((struct sockaddr *) &pointer->ai_addr), serverIp, INET6_ADDRSTRLEN);
    printf("Server IP is %s\n",serverIp);

    return listener_fd;

}
void add_to_poll_fds(struct pollfd *pollfds[], int newFd ,int *fd_count,int *fd_size){


    if(fd_size == fd_count){
        *fd_size *= 2;
        *pollfds = realloc(*pollfds, (sizeof (struct pollfd**)) * *fd_size);
    }

    (*pollfds)[*fd_count].fd = newFd;
    (*pollfds)[*fd_count].events = POLLIN;

    *fd_count += 2;

}

void del_from_poll_fds(struct pollfd *pollfds[], int index, int *fd_count){
    pollfds[index] = pollfds[*fd_count - 1];
    (*fd_count)--;

}


int main(){
    int fd_count;
    int fd_size = 5;
    int listener;
    struct pollfd *pollfds = malloc(sizeof (*pollfds) * fd_size);
    int new_fd;
    char path[MAX_PATH_SIZE];
    struct sockaddr_storage client_address;
    socklen_t client_addr_len;
    char buffer[BUFFSIZE];
    size_t bytes;
    char client_ip[INET6_ADDRSTRLEN];


    if(pollfds == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    listener = get_listener_socket();
    if(listener < 0){
        fprintf(stderr,"Error getting listener socket\n");
        exit(EXIT_FAILURE);
    }
    pollfds[0].fd = listener;
    pollfds[0].events = POLLIN;
    fd_count =1;
    printf("Server listener online\n");

    for(;;){
        int poll_count = poll(pollfds,fd_count, -1);

        memset(&buffer,0,sizeof buffer);
        memset(&bytes,0,sizeof bytes);

        if(poll_count < 0){
            perror("poll");
            exit(EXIT_FAILURE);
        }

        for (int (i) = 0; (i) < fd_count; ++(i)) {

            if(pollfds[i].revents == POLLIN){
                printf("POLLIN\n");
                if(pollfds[i].fd == listener){
                    new_fd = accept(listener,(struct sockaddr*)&client_address,&client_addr_len);
                    if(new_fd < 0){
                        perror("accept");
                        close(new_fd);
                        continue;
                    }

                    inet_ntop(client_address.ss_family,get_internet_address((struct sockaddr *) &client_address), client_ip, INET6_ADDRSTRLEN);

                    printf("New connection on socket %d from address %s\n",new_fd, client_ip);

                    add_to_poll_fds(&pollfds,new_fd,&fd_count,&fd_size);
                    strcpy(buffer,"You are now connected to the file transfer server\n");
                    if((bytes = send(new_fd,&buffer,sizeof buffer,0)) < 0){
                        perror("send");
                        continue;
                    }

/*
 * So instead of the usual echoing a message, we can use fwrite with an open file with the wb mode (write binary) to write everything we are receiving into a file of
 * the specified name. For this, I will keep it simple and just have 1 hardcoded location obviously if we have many connections we will need to take arguments from clients.
 * This is just an elementary example to show how a file can be transferred over TCP.
 *
 * The client will allow
 */
                } else{

                    printf("Got one\n");
                    FILE *file = fopen("/home/dustyn/file.txt", "wb");
                    if (file == NULL) {
                        perror("fopen");
                        // Handle error
                    }

                    while ((bytes = recv(pollfds[i].fd, buffer, BUFFSIZE, 0)) > 0) {
                        size_t bytes_written = fwrite(buffer, sizeof(char), bytes, file);
                        if (bytes_written != bytes) {
                            perror("fwrite");
                        }
                    }

                    if (bytes == -1) {
                        perror("recv");
                        // Handle error
                    }

                    if (bytes == 0) {
                        printf("Socket %d disconnected\n", pollfds[i].fd);
                        del_from_poll_fds(&pollfds, i, &fd_count);
                        fclose(file);
                        break;
                    }

                }
            }

        }
    }







}
