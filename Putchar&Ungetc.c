//
// Created by dustyn on 4/5/24.
//

/*
 * We can use putchar to put a single character into a stream, and use ungetc to put a character back into the buffer from which we took it, here is an example
 */
#include <stdio.h>

int putCharAndUnget(){
    //This will place individual characters into stdout
    putchar('H');
    putchar('e');
    putchar('l');
    putchar('l');
    putchar('o');

    //We can also use putc to specify the stream
    putc('\n',stdout);

    //unget can allow you to put something back into the stream buffer , and it will go back in at the beginning ready to be written first as the flush happens, notice the order when we then walk stdin and then
    //put each char in stdout, it goes back into the beginning of the buffer
    ungetc('1',stdin);
    ungetc('2',stdin);
    ungetc('3',stdin);

    printf("Now this will get these chars from stdin : \n");

    char c;
    while((c=getchar()) != EOF){
        putchar(c);
    }



}