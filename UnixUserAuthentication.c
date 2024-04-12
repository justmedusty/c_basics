//
// Created by dustyn on 4/7/24.
//
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "crypt.h"
#include <shadow.h>

/*
 * We can use the shadow.h and check the user entry from /etc/shadow to verify a users identity with the password, this could be used to create your own implemenation of ssh
 * or any other kind of remote shell operations. We will use shadow.h to get the user
 *
 * As is, this requires root to run properly but it will work. This can be used for a customized SSH protocol or any other kind of remote access protocol you wish to write
 */

#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

int authenticate_user(const char *username, const char *password) {
    //shadow password struct
    struct spwd *spw;
    //pointer of what will be our encrypted password
    char *encrypted_password;

    //This will get the username given and try to map it to a record in the shadow file
    spw = getspnam(username);

    //If is null, no such user
    if (spw == NULL) {
        fprintf(stderr, "User %s not found\n", username);
        exit(EXIT_FAILURE);
    }
    //doing this for compilation reasons i cant run other programs in my ide since the linker isnt working properly and have to do this manually
    //You need to add the commented out bit back in for this to work
    encrypted_password = "";//crypt(password, spw->sp_pwdp);

    //Make sure there was no errors in the crypt call
    if (encrypted_password == NULL) {
        perror("crypt");
        exit(EXIT_FAILURE);
    }
    //Compare the encrypted password to the record in the shadow file to see if it was correct
    if (strcmp(encrypted_password, spw->sp_pwdp) == 0) {
        printf("Authentication succeeded\n");
        return EXIT_SUCCESS;
    } else {
        printf("Authentication failed\n");
    }

    return EXIT_FAILURE;

}
/*
 * The main function which will be taking input from stdin and feeding it to our auth function which will attempt to authenticate the user based on
 * records in the shadow file, we will use fgets for line-at-a-time input
 */
int unixUserAuth(){

    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter username: ");
    fgets(username,sizeof username,stdin);
    username[strcspn(username, "\n")] = '\0';
    printf("Enter password: ");
    fgets(password,sizeof password,stdin);
    password[strcspn(password, "\n")] = '\0';



    if(authenticate_user(username,password) == 0){
        //handle remote shell stuff here
        return EXIT_SUCCESS;
    }else{
        //do not handle remote shell stuff
        return EXIT_FAILURE;
    }
}