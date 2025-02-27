#include "cmd.h"

uint8_t parseOptions(const char** arguments, uint8_t n)
{
    uint8_t options;

    for (int i = 1; i < n; ++i)
    {
        char* option = arguments[i];

        if (option[0] == '-')
        {
            switch (option[1])
            {
                case 'i':
                    break;
                case 'o':
                    break;
                case 'h':
                    options |= OPTION_HIDE;
                case 'e':
                    options |= OPTION_EXTRACT;
                    break;
                case 's':
                    options |= OPTION_STRING;
                    break;
                case 'f':
                    options |= OPTION_FILE;
                    break;
                default:
                    break;
            }
        }
    }

    return options;
}