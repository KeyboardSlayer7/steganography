#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "zlib.h"
#include "png.h"
#include "compression.h"
#include "bit_manipulation.h"
#include "global.h"

ARRAY_METHODS(Chunk);

void initPNG(PNG* png)
{
    png->header = NULL;
    initArray(&png->chunks, sizeof(Chunk));
    memset(&png->ihdr, 0, sizeof(IHDR)); 
    png->bytes_per_pixel = 0;
}

void loadPNG(PNG* png, const char* file_path)
{
    initPNG(png);

    FILE* file = fopen(file_path, "rb+");

    Chunk chunk;
    initChunk(&chunk);

    do 
    {
        char* ret = readPNGInChunks(file, &chunk); 

        if (ret != NULL)
        {
            // png->header = ret;
            
            png->header = malloc(sizeof(byte) * HEADER);
            memcpy(png->header, ret, HEADER); 
        }
        else
        {
            Chunk new_chunk;
            initChunk(&new_chunk);
            
            new_chunk.length = chunk.length;
            memcpy(new_chunk.type, chunk.type, 5); 
            
            new_chunk.data = malloc(sizeof(byte) * chunk.length);
            memcpy(new_chunk.data, chunk.data, sizeof(byte) * chunk.length);

            new_chunk.crc = chunk.crc;

            appendChunk(&png->chunks, new_chunk);

            if (!strcmp(new_chunk.type, "IHDR"))
            {
                memcpy(&png->ihdr, new_chunk.data, new_chunk.length);
                png->ihdr.width = byteswap(png->ihdr.width);
                png->ihdr.height = byteswap(png->ihdr.height);
            } 
        }

    } while (strcmp(chunk.type, "IEND") != 0);

    destroyChunk(&chunk); 

    png->bytes_per_pixel = bytesPerPixel(png->ihdr.color_type, png->ihdr.bit_depth);

    fclose(file);
}

void initChunk(Chunk* chunk)
{
    chunk->length = 0;
    chunk->type[4] = '\0';
    chunk->data = NULL;
    chunk->crc = 0;
}

void destroyChunk(Chunk* chunk)
{
    free(chunk->data);
}

void getDimensions(const PNG* png, int* width, int* height)
{
    Chunk ihdr = getChunk(&png->chunks, 0);
    memcpy(width, ihdr.data, sizeof(int));
    memcpy(height, ihdr.data + sizeof(int), sizeof(int));

    *width = byteswap(*width);
    *height = byteswap(*height);
}

void printIHDR(const IHDR* ihdr)
{
    printf(
        "-----IHDR-----\n"
        "Width: %d\n"
        "Height: %d\n"
        "Bit Depth: %d\n"
        "Color Type: %d\n"
        "Compression Method: %d\n"
        "Filter Method: %d\n"
        "Interlace Method: %d\n"
        "--------------\n",
        ihdr->width, ihdr->height, 
        ihdr->bit_depth, ihdr->color_type, 
        ihdr->compression_method, ihdr->filter_method, 
        ihdr->interlace_method
    );
}

uint8_t bytesPerPixel(ColorType color_type, uint8_t bit_depth)
{
    uint8_t channels;

    switch (color_type)
    {
        case INDEXED:
            channels = 1;
            break;
        case GRAYSCALE:
            channels = 1;
            break;
        case GRAYSCALE_ALPHA:
            channels = 2;
            break;
        case TRUE_COLOR:
            channels = 3;
            break;
        case TRUE_COLOR_ALPHA:
            channels = 4;
            break;
    }

    return (bit_depth * channels) / 8;
}

// Chunk must be idat or this function will fail
DataBlock* getRawPixelData(Chunk* chunk, const IHDR* ihdr)
{
    if (strcmp(chunk->type, "IDAT") != 0)
        return NULL;

    DataBlock original;
    original.length = chunk->length;
    original.data = chunk->data;

    // initDataBlock(&original, chunk->length);
    // memcpy(original.data, chunk->data, chunk->length); 

    int bytes_per_pixel = bytesPerPixel(ihdr->color_type, ihdr->bit_depth);
    int scanline_length = (ihdr->width * bytes_per_pixel) + 1; 
    int projected_size = scanline_length * ihdr->height;

    printf("Scanline length: %d, Projected size: %d\n", scanline_length, projected_size);

    DataBlock* inflated = zlibInflate(&original, projected_size); 
    DataBlock* unfiltered = removeFilter(inflated, scanline_length);

    destroyDataBlock(inflated);
    free(inflated);

    return unfiltered;
}

DataBlock* compressRawPixelData(DataBlock* raw, const IHDR* ihdr, int original_size)
{
    DataBlock original;
    original.length = raw->length;
    original.data = raw->data;

    int bytes_per_pixel = bytesPerPixel(ihdr->color_type, ihdr->bit_depth);
    int scanline_length = (ihdr->width * bytes_per_pixel) + 1;

    DataBlock* filtered = applyFilter(&original, scanline_length); 
    DataBlock* deflated = zlibDeflate(filtered, original_size);

    destroyDataBlock(filtered);
    free(filtered);

    return deflated;
}

// make sure file pointer is at 0 before first call to this function.
// this function skips the file's header (first 8 bytes).
// chunk must be properly initialised, or could lead to unwanted behaviour.
char* readPNGInChunks(FILE* file, Chunk* chunk)
{
    // skip header
    if (ftell(file) == 0)
    {
        // fseek(file, HEADER, SEEK_SET); code used to skip header, not anymore
        
        // this only reads properly with malloc, NO STATIC ARRAYS for some reason
        char* header = malloc(HEADER + 1);
        header[HEADER] = '\0';
        memset(header, 0, HEADER);

        fread(header, sizeof(byte), HEADER, file);

        return header;
    }

    fread(&chunk->length, sizeof(byte), sizeof(uint32_t), file);
    chunk->length = byteswap(chunk->length);

    fread(&chunk->type, sizeof(byte), 4, file);

    if (chunk->data == NULL)
    {
        chunk->data = malloc(sizeof(byte) * chunk->length);
        if (chunk->data == NULL)
        {
            printf("Malloc error!\n");
        }

        // memset(chunk->data, 0, sizeof(byte) * chunk->length);
    }
    else 
    {
        if (chunk->length > 0)
        {
            void* temp = realloc(chunk->data, chunk->length);
            if (temp != NULL)
            {
                chunk->data = temp;
                // memset(chunk->data, 0, chunk->length);
            }
            else
            {
                printf("Realloc error!\n");
            }
        }
    }

    fread(chunk->data, sizeof(byte), chunk->length, file); 

    fread(&chunk->crc, sizeof(byte), sizeof(uint32_t), file);
    chunk->crc = byteswap(chunk->crc);

    return NULL;
}

void hide(DataBlock* data, const char* string)
{
    // hide data length
    uint32_t length = strlen(string);
    byte* bytes_of_length = &length;

    for (int byte = 0; byte < sizeof(uint32_t); ++byte)
    {
        printf("Value of byte %d: %d\n", byte, bytes_of_length[byte]);

        for (int bit_pair = 0; bit_pair < BITS_IN_BYTE / 2; ++bit_pair)
        {
            int index = byte * sizeof(uint32_t) + bit_pair + 1;
            data->data[index] = setLeastSignificantBits(data->data[index], getNthBitPair(bytes_of_length[byte], bit_pair));
        } 
    }

    // hide data 

    for (int byte = 0; byte < length; ++byte)
    {
        for (int bit_pair = 0; bit_pair < BITS_IN_BYTE / 2; ++bit_pair)
        {
            int index = byte * length + bit_pair + 17;
            data->data[index] = setLeastSignificantBits(data->data[index], getNthBitPair(string[byte], bit_pair));
        }
    }

}

char* retrieve(DataBlock* data)
{
    // get length of data 
    uint32_t length = 0; 

    for (int byte = 0; byte < sizeof(uint32_t); ++byte)
    {
        for (int i = 0; i < BITS_IN_BYTE / 2; ++i)
        {
            length |= (uint32_t)getNthBitPair(data->data[byte * sizeof(uint32_t) + i + 1], 0) << (byte * sizeof(uint32_t) + i) * 2;
        }
    }

    printf("message of %d bytes\n", length);
   
    // get actual data
    byte* message = malloc(sizeof(byte) * length + 1);
    memset(message, 0, length);
    message[length] = '\0';

    for (int byte = 0; byte < length; ++byte)
    {
        for (int i = 0; i < BITS_IN_BYTE / 2; ++i)
        {
            message[byte] |= getNthBitPair(data->data[byte * length + i + 17], 0) << i * 2;
        } 
    }

    return (char*)message;
}

// data MUST be null terminated
void createModifiedPNGFile(PNG* png, const char* data)
{
    FILE* file = fopen("out.png", "wb+");

    fwrite(png->header, sizeof(byte), HEADER, file);

    uint32_t data_length = 0;

    if (data != NULL)
    {
        data_length = strlen(data);
        printf("message length: %d\n", data_length);
    }

    byte* data_length_bytes = &data_length;

    for (int i = 0; i < png->chunks.size; ++i)
    {
        Chunk chunk = getChunk(&png->chunks, i);

        if (!strcmp(chunk.type, "IDAT"))
        {
            FILE* another_file = fopen("original_deflated.txt", "wb+");
            fwrite(chunk.data, sizeof(byte), chunk.length, another_file);
            fclose(another_file);

            DataBlock* raw = getRawPixelData(&chunk, &png->ihdr);
            
            // printf("got raw pixel data\n");

            // hiding length of data
            // for (int int_byte = 0; int_byte < sizeof(uint32_t); ++int_byte)
            // {
            //     printf("byte: %d\n", (uint32_t)data_length_bytes[int_byte]);
            //     for (int pair = 0; pair < 8 / 2; ++pair)
            //     {
            //         int b = int_byte * 4 + pair + 1;
            //         byte changed_value = setLeastSignificantBits(raw->data[b], getNthBitPair(data_length_bytes[int_byte], pair));
                    
            //         printf("Changing byte: %d, from %d to %d\n", b, (uint8_t)raw->data[b], (uint8_t)changed_value);

            //         raw->data[b] = setLeastSignificantBits(raw->data[b], getNthBitPair(data_length_bytes[int_byte], pair));
            //     }
            // }

            // for (int i = 0; i < 16; ++i)
            // {
            //     printf("byte: %d\n", raw->data[i + 1]);
            // }

            hide(raw, data);

            DataBlock* deflated = compressRawPixelData(raw, &png->ihdr, chunk.length);
            
            chunk.length = deflated->length;
            chunk.data = deflated->data;
            uint32_t crc_temp = (0L, Z_NULL, 0);
            chunk.crc = crc32(crc_temp, chunk.data, chunk.length); 

            printf("CRC after: %d\n", chunk.crc);
        }

        uint32_t length = byteswap(chunk.length);
        uint32_t crc = byteswap(chunk.crc);

        fwrite((void*)&length, sizeof(byte), sizeof(uint32_t), file);
        fwrite((void*)chunk.type, sizeof(byte), 4, file);
        fwrite((void*)chunk.data, sizeof(byte), chunk.length, file);
        fwrite((void*)&crc, sizeof(byte), sizeof(uint32_t), file);
    }

    fclose(file);
}

char* retrieveFromPNGFile(PNG* png)
{
    char* output;

    for (int i = 0; i < png->chunks.size; ++i)
    {
        Chunk chunk = getChunk(&png->chunks, i);

        if (!strcmp(chunk.type, "IDAT"))
        {
            DataBlock* raw = getRawPixelData(&chunk, &png->ihdr);
            
            output = retrieve(raw);

            free(raw);
        }
    }

    return output;
}