#include <stdint.h>

#include "global.h"
#include "bit_manipulation.h"

byte getNthBitPair(byte value, uint8_t n)
{
    byte bitmask = 3 << (n * 2);
    byte temp = value & bitmask;
    byte ret = temp >> (n * 2);

    // printf("%lu\n", bitmask);

    return ret;
}

byte setLeastSignificantBits(byte value, byte new_value)
{
    byte output = value;
    output &= ~3;
    output |= new_value; 

    return output;
}