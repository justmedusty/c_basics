//
// Created by dustyn on 4/25/24.
//

#include <stdio.h>
#include "pthread.h"
#include "stdlib.h"
#include "unistd.h"

/*
 * Spin locks are another primitive used for handling concurrent access. You generally
 * will not see spinlocks too often in userspace. The way a spinlock works is that
 * when a lock is held by another process and a process trys the lock, it will
 * begin a busy loop constantly iterating over itself checking the lock. This works
 * well for operations that take very little time since the repetition will have little influence
 * on performance in such a small timeframe.
 *
 *
 * Another reason to use spinlocks is to keep the current waiting thread from being descheduled. If a process is sleeping (blocking)
 * it may get descheduled which can cause problems if that process was expected to execute within a certain timeframe.
 *
 * You will see a lot of spinlocks as you get into kernel programming, spinlock usage is littered throughout the linux
 * kernel. This is because these operations are executed very quickly and the locks are not held for long. This makes
 * spinlocks a good choice. Nevertheless we will go through spinlocks with pthreads in user-space. We can get into
 * spinlocks in kernel-space later.
 */

pthread_spinlock_t spinlock;
u_int8_t cannot_go_over_2 = 0;

void *pthread_function_example(){

    if(cannot_go_over_2 > 2){
        exit(EXIT_FAILURE);
    }

    pthread_spin_lock(&spinlock);
    write(1,"spin lock acquired\n\0",sizeof "spin lock acquired\n\0");

    cannot_go_over_2 ++;
    cannot_go_over_2 --;

    pthread_spin_unlock(&spinlock);

    if(cannot_go_over_2 > 2){
        exit(EXIT_FAILURE);
    }
    write(1,"spin lock released\n\0",sizeof "spin lock released\n\0");
    pthread_exit(EXIT_SUCCESS);
}


int main(){
    int i = 0;

    pthread_t threads[2];
    /*
     * private flag to not share with other processes, this is not
     * necessary here.
     */
    pthread_spin_init(&spinlock,PTHREAD_PROCESS_PRIVATE);
    while(i != 2){
        if (pthread_create(&threads[i],NULL,pthread_function_example,NULL) != 0){
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        i++;
    }

    i=0;

    while(i != 2){
        if(pthread_join(threads[i],NULL)!= 0){
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
        i++;
    }

    exit(EXIT_SUCCESS);

}