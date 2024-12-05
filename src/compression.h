#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <stdint.h>

#include "global.h"

typedef struct datablock_t 
{
    byte* data;
    uint32_t length;
} DataBlock;

// typedef Blob* BlobPtr;
typedef enum filtertype_t
{
    NONE,
    SUB,
    UP,
    AVERAGE,
    PAETH
} FilterType;

void initDataBlock(DataBlock* data_block, uint32_t size);
void destroyDataBlock(DataBlock* data_block);

DataBlock* zlibDeflate(const DataBlock* data, uint32_t original_size);
DataBlock* zlibInflate(const DataBlock* data, uint32_t projected_size);

DataBlock* applyFilter(DataBlock* raw_data, int scanline_length);
DataBlock* removeFilter(DataBlock* filtered_data, int scanline_length);

#endif