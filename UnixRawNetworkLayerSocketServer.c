
//
// Created by dustyn on 4/22/24.
//

#include <netdb.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include "sys/socket.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "poll.h"
#include "netinet/in.h"
#include "errno.h"
#include "string.h"

#define PORT "6969"
#define BACKLOG 15
#define PACKET_SIZE 1024
#define HEADER_SIZE 256

/*
 * These will be macros that will be used in message headers to signify to the other side of the association that
 * we have acknowledged, received bad data, disconnected, and our timeout value as well
 */

#define ACK "ACKNOWLEDGE"
#define CORRUPTION "BAD_DATA"
#define CLOSE "DISCONNECT"
#define TIMEOUT_MICROSECONDS 5000000 // 5 seconds
#define CHECKSUM_GOOD 0
#define CHECKSUM_NOT_GOOD (-1)

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

void *get_internet_addresses(struct sockaddr *sock_address) {

    if (sock_address->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sock_address)->sin_addr);
    } else {
        return &(((struct sockaddr_in6 *) sock_address)->sin6_addr);
    }
}

void signal_child_handler(int socket) {

    int saved_error = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_error;

}

int get_listener() {

    int listener;
    struct addrinfo hints, *server, *pointer;
    int yes = 1;

    hints.ai_socktype = SOCK_RAW;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_RAW;
    int return_value;

    if ((return_value = getaddrinfo(NULL, PORT, &hints, &server)) < 0) {
        gai_strerror(return_value);
        exit(EXIT_FAILURE);
    }

    for (pointer = server; pointer != NULL; pointer = pointer->ai_next) {

        if ((listener = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol) < 0)) {
            perror("socket");
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, pointer->ai_addr, pointer->ai_addrlen) < 0) {
            perror("bind");
            continue;
        }

        break;

    }

    if (pointer == NULL) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(server);

    if (listen(listener, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return listener;

}

/*
 * This will be our checksum function, it is going to iterate through bytes in the data and bitwise XOR them.
 * This means that if the bit matches, 0 is returned.
 * A non-zero value on verification indicates that bits have changed.
 * This is a simple way to implement a checksum in our home-grown transport layer.
 *
 * Visual Example:
 *
 * XORd checksum = 1001 0101
 * Received Data = 1011 0101
 * Result would  = 0010 0000
 *
 * The result is not 0
 * The data has changed
 *
 * Nice and simple, we like that around here
 */

uint16_t calculate_checksum(char *data[], size_t length) {

    uint16_t checksum = 0;

    for (size_t i = 0; i < length; i++) {
        char *byte = data[i];
        checksum ^= *byte;
    }
    return checksum;
}

/*
 * Here will be a function for verifying the checksum when we receive one in a client header message
 * It will return either 0 or -1, checksum good, or checksum not good.
 */

uint8_t compare_checksum(char *data[], size_t length, uint16_t received_checksum) {

    uint16_t new_checksum = calculate_checksum(&data, length);
    if ((new_checksum ^ received_checksum) != 0) {
        return (uint8_t) CHECKSUM_NOT_GOOD;
    } else {
        return (uint8_t) CHECKSUM_GOOD;
    }
}

/*
 * This is our conn handler function; since we are using raw sockets, there is no transport layer. WE are the transport layer. We will do
 * some basic headers to get some metadata about the incoming messages. There will be no retransmission automatically this is all done by the
 * transport layer. We can implement our own logic to resend messages after a specific timeout time, however for this we will keep it simple.
 * We will just tack some basic metadata like message size / message length. We could even do some kind of checksum. We can implement a basic
 * checksum with a simple bitwise XOR operation.
 *
 * If you do not remember, the bitwise XOR will compare 2 bits, let's say 0 and 1, and will return 1 if they were different, and 0 if they are the same.
 * This will handle basic data corruption. It, of course, is not foolproof, but it is very simple and very lightweight.
 *
 *
 *
 */

void handle_client_connection(int socket) {
    char msg_buffer[PACKET_SIZE];
    char hdr_buffer[HEADER_SIZE];
    struct msghdr;
    struct iovec iov[2];
    uint16_t checksum;
    const char welcome_msg[] = "Welcome to the raw socket server, we are building our own transport layer on top of the IP / network layer of the OSI !";
    checksum = (&welcome_msg, strlen(welcome_msg));
    uint16_t header = htons(checksum);
    iov[0].iov_base = &header;
    iov[0].iov_len = HEADER_SIZE;

    iov[1].iov_base = &welcome_msg;
    iov[1].iov_len = PACKET_SIZE;


}


int main() {

    int listener;
    char buffer[PACKET_SIZE];
    int new_fd;
    struct sockaddr_storage client_address;
    socklen_t addr_len;
    struct pollfd poll_listen;
    struct sigaction sa;
    char client_ip[INET6_ADDRSTRLEN];

    listener = get_listener();

    if (listener <= 0) {
        exit(EXIT_FAILURE);
    }

    poll_listen.fd = listener;
    poll_listen.events = POLLIN;

    if (poll(&poll_listen, 1, -1) < 0) {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = signal_child_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    for (;;) {

        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
            fprintf(stderr, "Error on signal action");
            exit(EXIT_FAILURE);
        }

        if (poll_listen.revents == POLLIN) {

            new_fd = accept(listener, (struct sockaddr *) &client_address, &addr_len);
            if (new_fd < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            inet_ntop(client_address.ss_family, get_internet_addresses((struct sockaddr *) &client_address), client_ip,
                      INET6_ADDRSTRLEN);
            printf("New connection on socket %d from address %s\n", new_fd, client_ip);

            pid_t child = fork();

            if (child < 0) {
                perror("fork");
            }
            if (child == 0) {
                handle_client_connection(new_fd);
                exit(EXIT_SUCCESS);
            } else {
                close(new_fd);
                continue;
            }


        }
    }


}