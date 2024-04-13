//
// Created by dustyn on 4/13/24.
//

/*
 * System V message queues are yet another way to achieve IPC in the UNIX environment.\
 * Message queues each have their own message and an integer type. Messages are retrieved in a FIFO fashion or by
 * their type. Reading from a message queue , as the name suggests, is message oriented. This means that you must read
 * by the message, you cannot read parts of a message you must read the entired message.
 * You call msgget() to get the handle however this is not the same as a file descriptor that we are used to.
 *
 * There are a few different behaviours worth noting when it comes to message queues.
 *
 * If you specify the ID on a rcv as 0, it will just rcv the most recent message of any message type
 * If you specify a non zero positve value it will rcv the most recent message from the queue with that message type
 * if you specify a negative value, we'll use -100 as an example. It will prioritize the queues by type in chronological order up to and including
 *
 * Here is a visualization of using -100 on the rcv
 *
 * Message 1 comes in : type 1
 * Message 2 copmes in : type 20
 *Message 3 comes in : type 55
 * Message 4 comes in : type 100
 * Message 5 does not come in because it is type 105 (greater than the specified 100
 * Now the recv will block
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include "sys/types.h"
#include "sys/msg.h"
#include "stdlib.h"

#define MSG_SIZE 1024

//You need to define a struct to hold your messages, this will generally be type long for the type, and the message which can be of any type you want
struct mbuf {
    long mtype;
    char mtext[MSG_SIZE];
} message;

int main() {

    //The key which will help us create our message queue
    key_t key;
    // the var which will hold the id our new message queue after the ftok call
    int msg_id;
    //This will be forking
    int child_pid;


    //Generate a key with ftok, specifying a path in /tmp , and proj is just a single char to identify this usage. Can be anything
    key = ftok("/tmp/queue", 'A');

    //Get our message queue id with msgget , pass our key, permission bits 0644 (rw-rw-r) and IPC_CREAT to create if not exists
    msg_id = msgget(key, 0664 | IPC_CREAT);

    //Fork it!
    child_pid = fork();
    //If error handle accordingly
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    //In the child process we want to load up a message struct with a message we wil send into the queue with the type 1
    if (child_pid == 0) {
        //this is the child
        message.mtype = 1;
        strlcpy(message.mtext,"child process checking in!\n",sizeof "child process checking in!\n");
        msgsnd(msg_id,&message,sizeof message,0);
        printf("Child sent message to queue\n");
        exit(EXIT_SUCCESS);
    } else{
        //parent

        //Wait for child to finish executing to reap the zombie process
        wait(NULL);
        //We will receive the next message with the type 1
        msgrcv(msg_id,&message,sizeof message, 1,0);

        //print the message we received from the queue.
        printf("Parent received this message from the child: %s\n",message.mtext);



    }
    //We will call msgctl with the command IPC_RMID to remove the id we created earlier
    msgctl(msg_id,IPC_RMID,NULL);
    exit(EXIT_SUCCESS);


}

