//
// Created by dustyn on 3/22/24.
//
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const unsigned int a = 0b1010; // 10 as binary
    const unsigned int b = 0b1111; //15 in binary

    //this is a bitwise AND which will compare each bit and return 1 if the same, 0 if not. This should leav us with 1010(10 in binary)
    const unsigned int c = a & b;
    printf("%d\n",c);

    //We can also shift bits to the left or to the right this will multiply a by 2^10 which should leave us with 327,680
    const int d = a << b;
    printf("%d\n",d);

    //We will shift these to the right which should leave us with 0 since we share shifting all of the bits into the void.
    const int e = a >> b;
    printf("%d\n",e);

    //however we can do this to shave off some bits withoout shaving every single one, this shift one bit to the right so we should be left with 101 (5)
    const int f = a >> 1;
    printf("%d\n",f);

    //another thing we can do is bitwise OR which, as I'm sure you can guess, will compare each bit and return 1 if any is 1 or 0 if they are both 0
    //this will result in 1111, 15 , the value of b, since no matter what one bit will be 1 dude to all 4 bits having a 1
    const int g = a | b;
    printf("%d\n",g);

    //we can also usse bitchwise XOR which will return 1 if both bits are different, 0 if they are the same
    // this will result in 0101,(5)
    const int h = a ^ b;
    printf("%d\n",h);

    //lastly we have the NOT which will flip each bit to 0 from 1 and to 1 from 0
    // this will result in 0101 again (5)
    // but it doesn't! it's -11. Which means that although I am delcaring unsigned inegers, they are being interpretted
    //as signed and using 2s compliment. This means there are 4 trailing 0s which are flipped, flipping the MSB turning the number
    //negative
    const unsigned int i = ~a;
    printf("%d\n",i);




}