#ifndef FILTER_H
#define FILTER_H

#include <stdbool.h>

#include "global.h"

typedef struct filterdata_t 
{
    byte* before;
    byte* after;
    int scanline;
    int scanline_length;
    int bpp;
    bool apply;
} FilterData;

// void filterNone();
uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c);

void filterSub(FilterData* filter_data);
void filterUp(FilterData* filter_data);
void filterAverage(FilterData* filter_data);
void filterPaeth(FilterData* filter_data);

void unfilterSub(FilterData* filter_data);
void unfilterUp(FilterData* filter_data);
void unfilterAverage(FilterData* filter_data);
void unfilterPaeth(FilterData* filter_data);

#endif // FILTER_H