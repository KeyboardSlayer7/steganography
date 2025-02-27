#include "math.h"
#include "filter.h"

/*
For these filter types:
    c, b
    a, x

    a = left
    b = above
    c = above left
    x = current
*/

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c)
{
    uint8_t p, pa, pb, pc, pr;

    p = a + b - c;
    pa = abs(p - a);
    pb = abs(p - b);
    pc = abs(p - c);

    if (pa <= pb && pa <= pc)
        pr = a;
    else if (pc <= pc)
        pr = b;
    else
        pr = c;

    return pr;
}

void filterSub(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;

    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;
        if (i < filter_data->bpp)
        {
            filter_data->after[current_index] = filter_data->before[current_index];
            continue;
        }

        byte a = filter_data->before[current_index - filter_data->bpp];
        byte x = filter_data->before[current_index];

        if (filter_data->apply)
        { 
            filter_data->after[current_index] = x - a;
        }
        else
        {
            filter_data->after[current_index] = filter_data->before[current_index] + filter_data->after[current_index - filter_data->bpp];
        }
    }
}

void filterUp(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;
    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;
        if (i == 0)
        {
            filter_data->after[current_index] = filter_data->before[current_index];
            continue;
        }

        byte b = filter_data->before[current_index - filter_data->scanline_length];
        byte x = filter_data->before[current_index];

        if (filter_data->apply)
        {
            filter_data->after[current_index] = x - b;
        }
        else
        {
            filter_data->after[current_index] = filter_data->before[current_index] + filter_data->after[current_index - filter_data->scanline_length];
        } 
    }
}

void filterAverage(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;
    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;

        byte a = filter_data->before[current_index - filter_data->bpp];
        byte b = filter_data->before[current_index - filter_data->scanline_length];
        byte x = filter_data->before[current_index];

        if (i < filter_data->bpp + 1)
        {
            if (i == 0)
            {
                filter_data->after[current_index] = filter_data->before[current_index];
                continue;
            }
            a = 0;
        }

        // printf("a: %d\n", a);

        if (filter_data->apply)
        {
            filter_data->after[current_index] = x - ((a + b) / 2);
        }
        else
        {
            if (i < filter_data->bpp + 1)
            {
                filter_data->after[current_index] = filter_data->before[current_index] + ((0 + filter_data->after[current_index - filter_data->scanline_length]) / 2);
            }
            else
            {
                filter_data->after[current_index] = filter_data->before[current_index] + ((filter_data->after[current_index - filter_data->bpp] + filter_data->after[current_index - filter_data->scanline_length]) / 2);
            }
        }
    }
}

void filterPaeth(FilterData* filter_data)
{
    int z = filter_data->scanline;
    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        if (i == 0)
        {
            filter_data->after[z + i] = filter_data->before[z + i];
            continue;
        }

        byte a = filter_data->before[z + i - filter_data->bpp];
        byte b = filter_data->before[z + i - filter_data->scanline_length];
        byte c = filter_data->before[z + i - filter_data->scanline_length - filter_data->bpp];
        byte x = filter_data->before[z + i];

        if (filter_data->apply)
        {
            filter_data->after[z + i] = x - paethPredictor(a, b, c);
        }
        else
        {
            filter_data->after[z + i] = filter_data->before[z + i] + paethPredictor(filter_data->after[z + i - filter_data->bpp], filter_data->before[z + i - filter_data->scanline_length], filter_data->before[z + i - filter_data->scanline_length - filter_data->bpp]);
        }
    }
}