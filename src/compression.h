#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdint.h>

#include "global.h"

/* 
TODO: 
1. Replace use of DataBlock with Array
*/

typedef struct datablock_t 
{
    byte* data;
    uint32_t length;
} DataBlock;

// typedef Blob* BlobPtr;
typedef enum filtertype_t
{
    NONE = 0,
    SUB = 1,
    UP = 2,
    AVERAGE = 3,
    PAETH = 4
} FilterType;

void initDataBlock(DataBlock* data_block, uint32_t size);
void destroyDataBlock(DataBlock* data_block);

DataBlock* zlibDeflate(const DataBlock* data, uint32_t original_size);
DataBlock* zlibInflate(const DataBlock* data, uint32_t projected_size);

DataBlock* applyFilter(DataBlock* raw_data, int scanline_length);
DataBlock* removeFilter(DataBlock* filtered_data, int scanline_length);

#endif