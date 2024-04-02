//
// Created by dustyn on 4/2/24.
//

/*
 * the send() system call will return how many bytes were sent, and on occasion it may not send every byte.
 * You cannot control this. The kernel acts independently and if for whatever reason it is executed without completely running your
 * buffer dry,You can use this code snippet below. Note this snippet does nothing on its own it needs to be added to one of the other
 * network socket files
 */


#include <sys/socket.h>
#include <sys/types.h>

//This is a function which could be used to ensure all bytes are sent in the case that the kernel does not complete
//the send for some reason or other
//We will pass our receiving socket fd, the buffer with the message, and the length of the message

int sendAllOfMessage(int socketFd,char *buffer, int *length){

    //We will keep track of every byte that was sent with totalSent
    int totalSent = 0;

    //We will keep track of bytes left to send with bytes left, of size length which was passed as the size of the message to be sent
    int bytesLeft = *length;

    //This will be the actual number sent
    int numberSent ;

    //While loop do not break until total sent is the length of the message
    while(totalSent < *length){

        //Attempt to send to to the target file descriptor with the message in buffer and bytes sent result store to numberSent.
        numberSent = send(socketFd,buffer,sizeof buffer,0);

        //Standard error checking
        if (numberSent == -1){
            break;
        }

        //Update totalSent and bytesLeft accordingly
        totalSent += numberSent;
        bytesLeft -= numberSent;

    }

    //Here will update length to be of size totalSent
    *length = totalSent;

    //Return -1 if numberSent is -1 (indicates error) , and 0 if not (successful call)
    return numberSent ==-1?-1:0;
}
