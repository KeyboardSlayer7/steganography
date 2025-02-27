#ifndef CMD_H
#define CMD_H

#include <stdint.h>

typedef enum optionflags_t
{
    OPTION_HIDE = 1,
    OPTION_EXTRACT = 2,
    OPTION_STRING = 4,
    OPTION_FILE = 8
} OptionFlags;

#define HIDE_STRING 5
#define HIDE_FILE 9
#define EXTRACTION 2

uint8_t parseOptions(const char** arguments, uint8_t n);

#endif