//
// Created by dustyn on 4/19/24.
//

#include <sys/wait.h>
#include <errno.h>
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "error.h"

/*
 * Record locking is a mechanism of locking portions (or all) of a file. This is accomplished with functions in the fcntrl collection.
 * You can lock an entire file or lock a specific byte range within the file. You can also lock past the end of the file, but cannot lock
 * before the beginning of the file. Record locking is sort of a misnomer because you are not locking a record you are locking a byte range.
 *
 * An important consideration here, these locks are advisory only. They only work when the other processes are aware of the locks and are checking. A process not
 * checking is not bound by any other processes locks. I will show you an example of checking the lock and then reading the locked file so you can see 1: how to check a lock,
 * and 2 that you can just access the file anyway. This sort of functionality is crucial for database systems. Record locking allows other processes to see if this one particular byte range
 * is being used by another process which can help coordinate the access in a way that removes race conditions.
 * Anyway
 * Let's go through this
 */

int record_locking() {

    int fd = open("/home/dustyn/dustyns_super_sensitive_data.txt", O_CREAT | O_RDWR | O_TRUNC);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    const char buf[] = "Record 1: data1\nRecord 2: data2\nRecord 3: data3\nRecord 4: PROTECTED\n\0";
    printf("Bytes written : %lu\n", strlen(buf));

    write(fd, &buf, strlen(buf));

    /*
     * We will create our file lock data structure here which can then be passed into fcntl()
     */

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start =  -1 * (long)(sizeof "Record 4: data1\n" - 1);
    lock.l_whence = SEEK_END;
    lock.l_len = sizeof "Record 4: data1\n" - 1;

    if (fcntl(fd, F_SETLK, &lock) != 0) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    printf("File locked\n");

    /*
     * Now that our lock is live, a read and a write lock, let's test it
     */

    pid_t child = fork();

    if (child == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child == 0) {

        /*
         * In our child process we are going to close the fd inherited from the parent image, and reopen it for a read session.
         * Read write flags
         *
         * We're first going to read the file to demonstrate that locks are advisory and not strictly enforced. The parent process
         * has a read lock currently on record 4, however we can read that just fine.
         *
         * Afterwards, we're going to try to acquire a write lock on the same location. This will fail. This is how we know there is a lock there
         * currently.
         */
        close(fd);

        printf("child opening file\n");

        fd = open("/home/dustyn/dustyns_super_sensitive_data.txt",   O_RDWR);

        char contents[1024];

        printf("Reading\n");

        ssize_t result = read(fd, &contents, sizeof (contents));

        contents[result] = '\0';

        printf("%s\n", contents);

        printf("child opened with ret val %lo\n", result);

        //Ensure we read something
        if (result == 0) {

            strerror((int) result);
            exit(EXIT_FAILURE);

            /*
             * Here we can see an example of what happens when we try to acquire a write lock, it will not
             * succeed since the parent currently holds a readlock for this same offset.
             * A write lock blocks all other locks so it holds priority. A previously held read lock
             * prevents a write lock. It does not prevent other read locks.
             */
        } else {

            lock.l_type = F_WRLCK;
            if (fcntl(fd,F_SETLK,&lock) != 0){

                perror("\nfcntl");
                exit(EXIT_FAILURE);

            }
            exit(EXIT_SUCCESS);
        }
    } else {

        /*
         * Wait until the child terminates, then unlock the section we previously locked
         */
        wait(NULL);
        printf("Child terminated\n");
        lock.l_type = LOCK_UN;
        if(fcntl(fd,F_UNLCK,&lock) != 0 ){
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
        printf("file unlocked");
        exit(EXIT_SUCCESS);
    }
}