#include <stdlib.h>
#include <stdint.h>

#include "compression.h"
#include "global.h"
#include "zlib.h"

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

    output = deflateInit(&stream, Z_BEST_COMPRESSION);

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

    return blob;
}

DataBlock* applyFilter(DataBlock* raw_data, int scanline_length)
{
    int scanlines = raw_data->length / scanline_length;

    DataBlock* filtered_data = malloc(sizeof(DataBlock));
    initDataBlock(filtered_data, raw_data->length);

    for (int i = 0; i < scanlines; ++i)
    {
        int row = i * scanline_length;
        switch ((FilterType)(raw_data->data[row]))
        {
            case NONE:
                break;
            case SUB:
                for (int j = 0; j < scanline_length; j++)
                {
                    if (j == 0 || j == 1)
                    {
                        filtered_data->data[row + j] = raw_data->data[row + j];
                    }
                    else
                    {
                        filtered_data->data[row + j] = raw_data->data[row + j] - raw_data->data[row + j - 4];
                    }
                }
                break;
            case UP:
                if (i == 0)
                {
                    break;
                }
                else
                {
                    for (int j = 0; j < scanline_length; j++)
                    {
                        if (j == 0)
                        {
                            filtered_data->data[row + j] = raw_data->data[row + j];
                        }
                        else
                        {
                            filtered_data->data[row + j] = raw_data->data[row + j] - raw_data->data[(row - scanline_length) + j];
                        }
                    }
                }
                break;
            case AVERAGE:
                if (i == 0)
                {
                    break;
                }
                else 
                {
                    for (int j = 0; j < scanline_length; j++)
                    {
                        if (j == 0 || j == 1)
                        {
                            filtered_data->data[row + j] = raw_data->data[row + j];
                        }
                        else
                        {
                            filtered_data->data[row + j] = raw_data->data[row + j] - ((raw_data->data[row + j - 1] + raw_data->data[(row - scanline_length) + j]) / 2);
                        }
                    }
                }
                break;
            case PAETH:
                // printf("PAETH has not been implemented\n");
                
                for (int j = 0; j < scanline_length; j++)
                {
                    uint8_t a, b, c, p, pa, pb, pc, pr;

                    c = raw_data->data[(row - scanline_length) + j - 1];
                    b = raw_data->data[(row - scanline_length) + j];
                    a = raw_data->data[row + j - 1];

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

                    filtered_data->data[row + j] = raw_data->data[row + j] - pr;
                }

                return NULL;
                break;
        }
    } 

    return filtered_data;
}

// Returned DataBlock* was created using initDataBlock thus returned object must be freed using destroyDataBlock
DataBlock* removeFilter(DataBlock* filtered_data, int scanline_length)
{
    int scanlines = filtered_data->length / scanline_length;
    // printf("filtered data length: %d\n", filtered_data->length);

    DataBlock* raw_data = malloc(sizeof(DataBlock));
    initDataBlock(raw_data, filtered_data->length);

    for (int i = 0; i < scanlines; ++i)
    {
        int row = i * scanline_length; 
        switch ((FilterType)(filtered_data->data[row]))
        {
            case NONE:
                break;
            case SUB:
                for (int j = 0; j < scanline_length; j++)
                {
                    if (j == 0 || j == 1)
                    {
                        raw_data->data[row + j] = filtered_data->data[row + j];
                    }
                    else
                    {
                        raw_data->data[row + j] = filtered_data->data[row + j] + raw_data->data[row + j - 4];
                    }
                }
                break;
            case UP:
                if (i == 0)
                {
                    break;
                }
                else
                {
                    for (int j = 0; j < scanline_length; j++)
                    {
                        if (j == 0)
                        {
                            raw_data->data[row + j] = filtered_data->data[row + j];
                        }
                        else
                        {
                            raw_data->data[row + j] = filtered_data->data[row + j] + raw_data->data[(row - scanline_length) + j];
                        }
                    }
                }
                break;
            case AVERAGE:
                if (i == 0)
                {
                    break;
                }
                else 
                {
                    for (int j = 0; j < scanline_length; j++)
                    {
                        if (j == 0 || j == 1)
                        {
                            raw_data->data[row + j] = filtered_data->data[row + j];
                        }
                        else
                        {
                            raw_data->data[row + j] = filtered_data->data[row + j] + ((raw_data->data[row + j - 1] + raw_data->data[(row - scanline_length) + j]) / 2);
                        }
                    }
                }
                break;
            case PAETH:
                // printf("PAETH has not been implemented\n");
                
                for (int j = 0; j < scanline_length; j++)
                {
                    uint8_t a, b, c, p, pa, pb, pc, pr;

                    c = raw_data->data[(row - scanline_length) + j - 1];
                    b = raw_data->data[(row - scanline_length) + j];
                    a = raw_data->data[row + j - 1];

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

                    raw_data->data[row + j] = filtered_data->data[row + j] + pr;
                }

                return NULL;
                break;
        }
    } 

    return raw_data;
}