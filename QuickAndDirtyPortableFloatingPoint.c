//
// Created by dustyn on 4/2/24.
//

/*
 * Portability can be a huge issue when dealing with systems programming. One of these potential issues is using
 * floating points since there is no function such as ntoh/hton to convert these into network byte order to allow them
 * to be sent in a portable manner. If you just send them raw , the receiver may be on a system of another endianness,
 * it may also use a different size to deal with floats, lots of things can go wrong. THis is an example of a quick and dirty way to
 * write a converter for sending off to a client of another architecture and not having to worry as much. This isn't a great solution
 * but it's just an example of something
 */

#include <stdint.h>

// Function to convert a float to network byte order
uint32_t htonf(float yourFloat) {
    uint32_t point;     // Initialize a uint32_t variable to hold the result
    uint32_t sign;      // Initialize a uint32_t variable to hold the sign of the float

    // Checking if the float is negative
    if (yourFloat < 0) {
        sign = 1;   // If negative, set the sign bit to 1
        yourFloat = -yourFloat; // Take the absolute value of the float
    } else {
        sign = 0;   // If positive, set the sign bit to 0
    }

    // Encoding the float into a uint32_t value
    point = (((uint32_t) yourFloat & 0x7fff) << 16) | (sign << 31);
    // Extracting the fractional part of the float and encoding it into the uint32_t value
    point |= (uint32_t)(((yourFloat - (int)yourFloat) * 65536.0f)) & 0xffff;
    return point; // Return the encoded uint32_t value
}

// Function to convert a uint32_t value from network byte order to a float
float ntohf(uint32_t point) {
    // Decoding the uint32_t value into a float
    float yourFloat = ((point >> 16)&0x7fff);
    // Adding the fractional part to the float
    yourFloat += (point&0xffff) / 65536.0f;
    // Checking if the float was originally negative
    if ((point >> 31) & 0x1) {
        yourFloat = -yourFloat; // If originally negative, make the float negative
    }
    return yourFloat; // Return the decoded float value
}