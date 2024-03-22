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

    //We can also shift bits to the left or to the right this will multiplty a by 2^10 which should leave us with 327,680
    const int d = a << b;
    printf("%d\n",d);

    //We will shift these to the right which should leave us with 0 since we share shifting all of the bits out of existence.
    const int e = a >> b;
    printf("%d\n",e);

    //however we can do this to shave off some bits withoout shaving every single one, this shift one bit to the right so we should be left with 101 (5)
    const int f = a >> 1;
    printf("%d\n",f);

    //another thing we can do is bitwise OR which, as I'm sure you can guess, will compare each bit and return 1 if any is 1 or 0 if they are both 0
    //this will result in 1111, 15 , the value of b, since no matter what one bit will be 1 dude to all 4 bits having a 1
    const int g = a | b;
    printf("%d\n",g);


}