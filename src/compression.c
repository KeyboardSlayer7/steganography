#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "compression.h"
#include "global.h"
#include "zlib.h"
#include "filter.h"

void initDataBlock(DataBlock* data_block, uint32_t size)
{
    data_block->data = malloc(sizeof(byte) * size);
    if (data_block->data == NULL)
    {
        printf("Memory for DataBlock could not be allocated\n");
    }

    data_block->length = size;
}

void destroyDataBlock(DataBlock* data_block)
{
    free(data_block->data);
}

// Returned DataBlock* was created using initDataBlock thus returned object must be freed using destroyDataBlock
DataBlock* zlibDeflate(const DataBlock* data, uint32_t original_size)
{
    int output;
    z_stream stream;
    DataBlock* blob = malloc(sizeof(DataBlock));

    // blob.length = original_size + KILO_BYTE;                     has been replaced by initDataBlock
    // blob.data = malloc(sizeof(byte) * blob.length);              

    initDataBlock(blob, original_size); // + KILO_BYTE);

    stream.zalloc = Z_NULL; 
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL; 
    
    stream.avail_in = data->length;
    stream.next_in = data->data;
    
    stream.avail_out = blob->length; 
    stream.next_out = blob->data;

    output = deflateInit(&stream, Z_BEST_COMPRESSION); // Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
    // deflateParams(&stream, Z_DEFAULT_COMPRESSION, Z_FILTERED);

    if (output != Z_OK)
    {
        return NULL;
    }

    do 
    {
        output = deflate(&stream, Z_FINISH);
    } while (output != Z_STREAM_END);

    output = deflateEnd(&stream);

    if (output != Z_OK)
    {
        return NULL;
    }

    FILE* file = fopen("deflated.txt", "wb+");

    fwrite(blob->data, sizeof(byte), blob->length, file);

    fclose(file);

    return blob;
}

// Returned DataBlock* was created using initDataBlock thus returned object must be freed using destroyDataBlock
DataBlock* zlibInflate(const DataBlock* data, uint32_t projected_size)
{
    int output;
    z_stream stream;
    DataBlock* blob = malloc(sizeof(DataBlock));

    // blob.length = projected_size + KILO_BYTE;                    has been replaced by initDataBlock
    // blob.data = malloc(sizeof(byte) * blob.length);

    initDataBlock(blob, projected_size); // + KILO_BYTE);

    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    stream.avail_in = data->length;
    stream.next_in = data->data;

    stream.avail_out = blob->length;
    stream.next_out = blob->data;

    output = inflateInit(&stream);

    if (output != Z_OK)
    {
        printf("inflateInit() returned %d\n", output);
        return NULL;
    }

    do
    {
        output = inflate(&stream, Z_FINISH); 
    } while(output != Z_STREAM_END);

    output = inflateEnd(&stream);

    if (output != Z_OK)
    {
        return NULL;
    }

    // printf("avail_out: %d, avail_in: %d, given: %d\n", stream.avail_out, stream.avail_in, blob->length);

    FILE* file = fopen("inflated.txt", "wb+");
    
    fwrite(blob->data, sizeof(byte), blob->length, file);

    fclose(file);

    return blob;
}

DataBlock* applyFilter(DataBlock* raw_data, int scanline_length)
{
    int scanlines = raw_data->length / scanline_length;

    DataBlock* filtered_data = malloc(sizeof(DataBlock));
    initDataBlock(filtered_data, raw_data->length);

    FilterData filter_data;
    filter_data.before = raw_data->data;
    filter_data.after = filtered_data->data;
    filter_data.scanline_length = scanline_length;
    filter_data.bpp = 4;
    filter_data.apply = true;
    
    for (int i = 0; i < scanlines; i++)
    {
        filter_data.scanline = i * scanline_length;
        // printf("Scanline: %d, index: %d, value: %d\n", i, filter_data.scanline, raw_data->data[filter_data.scanline]);

        switch (raw_data->data[i * scanline_length])
        {
            case NONE:
                // filterNone(&filter_data);
                break;
            case SUB:
                filterSub(&filter_data);
                break;
            case UP:
                filterUp(&filter_data);
                break;
            case AVERAGE:
                filterAverage(&filter_data);
                break;
            case PAETH:
                filterPaeth(&filter_data);
                break;
        }
    }

    FILE* file = fopen("filtered.txt", "wb+");
    
    fwrite(filtered_data->data, sizeof(byte), filtered_data->length, file);

    fclose(file);

    return filtered_data;
}

// Returned DataBlock* was created using initDataBlock thus returned object must be freed using destroyDataBlock
DataBlock* removeFilter(DataBlock* filtered_data, int scanline_length)
{
    int scanlines = filtered_data->length / scanline_length;
    // printf("filtered data length: %d\n", filtered_data->length);

    DataBlock* raw_data = malloc(sizeof(DataBlock));
    initDataBlock(raw_data, filtered_data->length);

    FilterData filter_data;
    filter_data.before = filtered_data->data;
    filter_data.after = raw_data->data;
    filter_data.scanline_length = scanline_length;
    filter_data.bpp = 4;
    filter_data.apply = false;

    for (int i = 0; i < scanlines; i++)
    {
        filter_data.scanline = i * scanline_length;
        // printf("Scanline: %d, index: %d, value: %d\n", i, filter_data.scanline, filtered_data->data[filter_data.scanline]);

        switch (filtered_data->data[i * scanline_length])
        {
            case NONE:
                // filterNone(&filter_data);
                break;
            case SUB:
                filterSub(&filter_data);
                break;
            case UP:
                filterUp(&filter_data);
                break;
            case AVERAGE:
                filterAverage(&filter_data);
                break;
            case PAETH:
                filterPaeth(&filter_data);
                break;
        }
    }

    return raw_data;
}