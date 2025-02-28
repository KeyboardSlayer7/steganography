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

        byte x = filter_data->before[current_index];
        byte a = filter_data->before[current_index - filter_data->bpp];

        if (i < filter_data->bpp)
        {
            filter_data->after[current_index] = x;
            continue;
        }
  
        filter_data->after[current_index] = x - a;         
    }
}

void unfilterSub(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;

    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;

        byte x = filter_data->before[current_index];
        byte a = filter_data->after[current_index - filter_data->bpp];

        if (i < filter_data->bpp)
        {
            filter_data->after[current_index] = x;
            continue;
        }

        filter_data->after[current_index] = x + a;
    }
}

void filterUp(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;

    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;
        
        byte x = filter_data->before[current_index];
        byte b = filter_data->before[current_index - filter_data->scanline_length];

        if (i == 0)
        {
            filter_data->after[current_index] = x;
            continue;
        }

        filter_data->after[current_index] = x - b; 
    }
}

void unfilterUp(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;

    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;

        byte x = filter_data->before[current_index];
        byte b = filter_data->after[current_index - filter_data->scanline_length];

        if (i == 0)
        {
            filter_data->after[current_index] = x;
            continue;
        }

        filter_data->after[current_index] = x + b;
    }
}

void filterAverage(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;

    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;

        byte x = filter_data->before[current_index];
        byte a = filter_data->before[current_index - filter_data->bpp];
        byte b = filter_data->before[current_index - filter_data->scanline_length];

        if (i < filter_data->bpp + 1)
        {
            if (i == 0)
            {
                filter_data->after[current_index] = x;
                continue;
            }

            a = 0;
        }

        // printf("a: %d\n", a); 
        filter_data->after[current_index] = x - ((a + b) / 2);
    }
}

void unfilterAverage(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;
    
    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;

        byte x = filter_data->before[current_index];
        byte a = filter_data->after[current_index - filter_data->bpp];
        byte b = filter_data->after[current_index - filter_data->scanline_length];
        
        if (i < filter_data->bpp + 1)
        {
            if (i == 0)
            {
                filter_data->after[current_index] = x;
                continue;
            }
            
            a = 0;
        }

        filter_data->after[current_index] = x + ((a + b) / 2); 
    }
}

void filterPaeth(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;

    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;

        byte x = filter_data->before[current_index];
        byte a = filter_data->before[current_index - filter_data->bpp];
        byte b = filter_data->before[current_index - filter_data->scanline_length];
        byte c = filter_data->before[current_index - filter_data->scanline_length - filter_data->bpp];

        if (i == 0)
        {
            filter_data->after[current_index] = x;
            continue;
        }

        filter_data->after[current_index] = x - paethPredictor(a, b, c); 
    }
}

void unfilterPaeth(FilterData* filter_data)
{
    int current_scanline_start = filter_data->scanline;
    
    for (int i = 0; i < filter_data->scanline_length; i++)
    {
        int current_index = current_scanline_start + i;

        byte x = filter_data->before[current_index];
        byte a = filter_data->after[current_index - filter_data->bpp];
        byte b = filter_data->after[current_index - filter_data->scanline_length];
        byte c = filter_data->after[current_index - filter_data->scanline_length - filter_data->bpp];       
       
        if (i == 0)
        {
            filter_data->after[current_index] = x;
            continue;
        }
        
        filter_data->after[current_index] = x + paethPredictor(a, b, c);
    } 
}