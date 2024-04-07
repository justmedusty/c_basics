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
 * For some reason this doesn't link properly when run in CLion so I need to compile it from the command line with the
 * -lcrypt link flag to make sure it is linked
 */

#define MAX_USERNAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100

int authenticate_user(const char *username, const char *password) {
    struct spwd *spw;
    char *encrypted_password;


    spw = getspnam(username);

    if (spw == NULL) {
        fprintf(stderr, "User %s not found\n", username);
        exit(EXIT_FAILURE);
    }

    encrypted_password = crypt(password, spw->sp_pwdp);
    printf("Stored:%s\n",spw->sp_pwdp);
    printf("Given:%s\n",encrypted_password);

    printf("%s\n",encrypted_password);
    if (encrypted_password == NULL) {
        perror("crypt");
        exit(EXIT_FAILURE);
    }

    if (strcmp(encrypted_password, spw->sp_pwdp) == 0) {
        printf("Authentication succeeded\n");
        return EXIT_SUCCESS;
    } else {
        printf("Authentication failed\n");
    }

    return EXIT_FAILURE;

}

int main(){

    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter username\n");
    fgets(username,sizeof username,stdin);
    username[strcspn(username, "\n")] = '\0';
    printf("Enter password\n");
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