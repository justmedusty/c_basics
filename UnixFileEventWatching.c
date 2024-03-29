//
// Created by dustyn on 3/29/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include <sys/inotify.h>

//IeventQueue,IeventStatus, readLength which we will need shortly
int IeventQueue = -1;
int IeventStatus = -1;
int readLength;
int unixFileEventWatching(int argc, char** argv)
{
    //We're going to define our watch masks , which will indicate what state the file is currently in, being accessed, deleted, modified, moved etc using the constant values defined in inotify.h header file
    const uint32_t watchMask = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE | IN_MODIFY | IN_MOVE_SELF;
    //Base path for accessing the file in question
    char *basePath = NULL;
    //Token will be used to split up
    char *token = NULL;
    //This is the watch even that we will be monitoring, we will just be monitoring one file at a time
    const struct inotify_event* watchEvent;
    //This is the pointer to the address at which we will hold the value of the notification of what event just happened to the file
    char *notificationMessage;


    //4 kilobyte buffer
    char buffer[4096];
    //We will do an arg check to make sure the path is there, if not exit on exit failure
    if (argc < 2){
        fprintf(stderr,"USAGES : inotify PATH\n");
        exit(EXIT_FAILURE);
    }
    //Base path is going to be dynamically allocated the size of the argument given + 1 byte
    basePath = (char *)malloc((sizeof(char) *(strlen(argv[1]) + 1 )));
    //Copy the argument into basepath now that we have called malloc and allocated memory on the heap
    strcpy(basePath,argv[1]);
    //This will help p
    token = strtok(basePath,"/");
    while (token != NULL) {
        basePath = token;
        token = strtok(NULL,"/");
    }
    //Check that basepath is not null , error handle accordingly
    if (basePath == NULL){
        fprintf(stderr,"Error getting base path \n");
        exit(EXIT_FAILURE);
    }
    //print the base path
    printf("%s\n", basePath);
    //We will initialize our inotify queue by utilizing the inotify_init() system call
    IeventQueue = inotify_init();

    //We will check that the return value was not -1 which indicates error, and handle such error accordingly
    if (IeventQueue == -1){
        fprintf(stderr,"Error initializing inotifiy instance \n");
        exit(EXIT_FAILURE);
    }
    //Now we will fill our IevenStatus by calling the inotify_add_watch system call, and passing the event queue, the path argument,and the watch masks which will tell us which event has occurred
    IeventStatus = inotify_add_watch(IeventQueue, argv[1], watchMask);
    //Make sure that the return value of inotify_add_watch does not indicate an error, if it does, act accordingly
    if (IeventStatus == -1){
        fprintf(stderr,"Error adding file to watch instance");
        exit(EXIT_FAILURE);


    }

    while(true){
        //We we will keep tie our readLength to the IeventQueue , and send the output into our buffer
        readLength = read(IeventQueue, buffer, sizeof(buffer));
        //Standard error checking, exit on error
        if(readLength == -1){
            exit(EXIT_FAILURE);
        }
        //Here we will walk the buffer through bufferpointer to process each incoming event
        for (char *bufferPointer = buffer; bufferPointer < buffer + readLength; bufferPointer += sizeof(struct inotify_event) + watchEvent->len){

            //cast watch event to the event in the current iteration of the buffer via buffer pointer
            watchEvent = (const struct inotify_event *) bufferPointer;

            //For each event it could be , print said event
            //If even is NULL, iterate the loop and continue
            if (watchEvent->mask & IN_CREATE) {
                notificationMessage = "File created\n";
            }
            if (watchEvent->mask & IN_DELETE) {
                notificationMessage = "File deleted\n";
            }
            if (watchEvent->mask & IN_ACCESS ){
                notificationMessage = "File accessed\n";
            }
            if (watchEvent->mask & IN_CLOSE_WRITE) {
                notificationMessage = "File writen and closed\n";
            }
            if (watchEvent->mask & IN_MODIFY) {
                notificationMessage = "File modified\n";
            }
            if (watchEvent->mask & IN_MOVE_SELF) {
                notificationMessage = "File moved\n";
            }

            if (notificationMessage == NULL){
                continue;
            }

            printf("%s\n",notificationMessage);




        }

    }

}

//This is the inotify unix system interface which will allow you to monitor file events in your systems programming endeavours !