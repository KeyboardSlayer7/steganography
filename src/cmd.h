#ifndef CMD_H
#define CMD_H

#include <stdint.h>

typedef enum optionflags_t
{
    HIDE = 1,
    EXTRACT = 2,
    STRING = 4,
    FILE = 8
} OptionFlags;

#define HIDE_STRING 5
#define HIDE_FILE 9
#define EXTRACT 2

uint8_t parseOptions(const char** arguments, uint8_t n);

#endif