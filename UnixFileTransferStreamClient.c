//
// Created by dustyn on 4/21/24.
//

#define PORT "6969"

#include <netdb.h>
#include <string.h>
#include "sys/socket.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#define BUFFSIZE 4096
int main1(int argc, char *argv[]) {

    if (argc < 3) {
        fprintf(stderr, "Usage: ftcp server_ip file_path");
    }
    int sock_fd;
    struct addrinfo hints, *pointer, *server;
    int return_value;
    char buffer[BUFFSIZE];


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((return_value = getaddrinfo(argv[1], PORT, &hints, &server)) != 0) {
        fprintf(stderr, "Error get server address info with error %s\n", gai_strerror(return_value));
        exit(EXIT_FAILURE);
    }

    for (pointer = server; pointer != NULL; pointer = pointer->ai_next) {
        if ((sock_fd = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol)) == -1) {
            perror("Couldnt make socket and get file descriptor from kernel\n");
            continue;
        }
        break;
    }

    if (pointer == NULL) {
        fprintf(stderr, "Error in creating socket\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(server);

    FILE *file = fopen(argv[2], "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    printf("Sending file\n");

    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), BUFFSIZE, file)) > 0) {
        ssize_t bytes_sent = send(sock_fd, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("Error sending data over socket");
            exit(EXIT_FAILURE);
        }
    }
    printf("sent file\n");

    if (ferror(file)) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }

    fclose(file);

    exit(EXIT_SUCCESS);


}
