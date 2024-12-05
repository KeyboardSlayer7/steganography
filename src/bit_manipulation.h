#ifndef BIT_MANIPULATION_H
#define BIT_MANIPULATION_H

#include <stdint.h>

#include "global.h"

byte getNthBitPair(byte value, uint8_t n);
byte setLeastSignificantBits(byte value, byte new_value);

#endif