#include <stdint.h>
#include <stdlib.h>

#include "global.h"

void initArray(Array* array, uint8_t element_size)
{
    array->data = malloc(10 * element_size);
    memset(array->data, 0, 10 * element_size);

    array->capacity = 10;
    array->size = 0;
}
