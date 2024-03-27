//
// Created by dustyn on 3/27/24.
//

#include <stdio.h>

//Endianness refers to where the MSB (most significant bit) is stored in a system. Generally when we read binary right to left, with the MSB being on the left,
//is little endian, and big endian would be the MSB being on the right side
//Don't understand? No problem
// take this binary byte : 0000 0001 this is one in a small endian system
//However representing this in a big endian system it would be represented like this : 1000 0000
//It is flipped! Our original byte , 0000 0001 would be either 127 or 256 on a big endian system depending on whether this integer is signed or not
//Big endian is not very common , mostly on older systems , however it does exist and is something that occasionally becomes relevant
int main(){
    //We will start with our little endian byte
    unsigned int littleEndian;
    //initialize an empty 8 bit integer which we will iterate through below
    unsigned int bigEndian = 0b00000000;
    //the number 213 (unsigned) in little endian format
    littleEndian = 0b11010101;

    //Now let's say we are transferring this data to a big endian system, the order of these bits need to be reversed!
    //How exactly do we do that ?
    //We can use the bitshift operator to accomplish this!

    for(int i = 0;i < 8;i++){
        //Confusing right? What we are doing here involves 4 bitwise operations. We need our little endian number (0b11010101) to be big endian (0xb10101011) Reversing the order of the bits
        //We are using a bitwise OR, a leftward bitshift a bitwise AND , and a rightward bitshift.
        //What we are doing is extracting each bit starting from the small endian LSB (the least significant bit) and moving it to the other side which is where the LSB is in a big endian byte.
        bigEndian |= ((littleEndian >> i)&1) << (7 -i);
    }
    //This should print 171 (unsigned) if correct (0xb10101011)
    printf("%u\n",bigEndian);
    //EH VOILÀ ! 171 ! would you look at that, we have now just converted a small endian byte to a big endian byte ! This can be very useful for transferring data from a small endian to a big endian system!
}