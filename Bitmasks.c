//
// Created by dustyn on 4/20/24.
//
/*
 * Bitmasks are a way of storing condensed information in a group of bits. It is storing information directly in indivudal bits
 * as opposed to using large data structures like chars and ints. Why use many bits for one flag when it can be set in a single bit?
 */
#include "sys/types.h"
#include "stdio.h"

int main(){
    /*
     * We're going to set the 3rd most significant bit to one, we can check whether this one bit is set by
     * bitwise AND alongside (1 << 5) which takes 1 and moves it 5 bits to the left leaving us with the same value (0010 0000)
     * The bitwise AND will then return 32 (the integer value) if that bit is set, and 0 if it is not! We can also set bits with a bitwise OR , or clear it with a bitwise NOT
     */
    u_int8_t binary = 0b00100000;
    int result = binary & (1 << 5);
    //the bit is set so it will print the integer value, 32, otherwise it would print 0
    printf("result %d\n",result);


    /*
     * Now we will set 2nd to showcase setting bits in the mask
     * This will make the integer value of the byte 36 if the bit is set successfully
     */
    binary |= (1 << 2);
    printf("result %d\n",binary);

    /*
     * We'll add an extra 1 to our bitmask because why!
     * The 1<<0 is redundant you can just put 1 as you can see below
     */
    binary |= (1 << 0);
    printf("result %d\n",binary);

    /*
     * Now let's clear these two bits in the mask that we just set. Back to our initial value
     * This works by bitwise NOTing 1 and 1 << 2 (0000 0100) which will end up being (1111 1110) and (111 1011) respectively
     * What you end up with is that one single bit ending up 0 because it was set before, and when bitwise AND is applied
     * to binary we end up with 0010 0101 AND 1111 1110 which leaves us with 0010 0100
     * And again with 0010 0100 AND 1111 1011 which leaves us with , you guessed it, 0010 0000! Our initial value!
     */

    binary &= ~1;
    printf("result %d\n",binary);

    binary &= ~(1 << 2);
    printf("result %d\n",binary);

}