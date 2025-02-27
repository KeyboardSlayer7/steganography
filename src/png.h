#ifndef PNG_H
#define PNG_H

#include <stdio.h>
#include <stdint.h>

#include "global.h"
#include "compression.h"

// data member must be set to NULL when struct is created. 
// type should be NULL terminated.
typedef struct chunk_t
{
    uint32_t length, crc;
    char type[5];
    byte* data;
} Chunk;

// maybe pack this for simple memcpy, from image buffer?
typedef struct ihdr_t
{
    uint32_t width, height;
    uint8_t bit_depth, color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
} IHDR;

typedef enum colortype_t 
{
    GRAYSCALE = 0,
    TRUE_COLOR = 2,
    INDEXED = 3,
    GRAYSCALE_ALPHA = 4,
    TRUE_COLOR_ALPHA = 6
} ColorType;

typedef enum criticalchunks_t
{
    CHUNK_IHDR,
    CHUNK_PLTE,
    CHUNK_IDAT,
    CHUNK_IEND
} CriticalChunks;

typedef struct png_t
{
    byte* header;
    IHDR ihdr;
    Array chunks;
    size_t bytes_per_pixel; 
} PNG;

void initPNG(PNG* png);
void loadPNG(PNG* png, const char* file_path);

void initChunk(Chunk* chunk);
void destroyChunk(Chunk* chunk);

void printIHDR(const IHDR* ihdr);

void getDimensions(const PNG* png, int* width, int* height);
uint8_t bytesPerPixel(ColorType color_type, uint8_t bit_depth);

DataBlock* getRawPixelData(Chunk* chunk, const IHDR* ihdr);
DataBlock* compressRawPixelData(DataBlock* raw, const IHDR* ihdr, int original_size);

void hide(DataBlock* data, const char* string);
char* retrieve(DataBlock* data);

char* readPNGInChunks(FILE* file, Chunk* chunk);
void createModifiedPNGFile(PNG* png, const char* data);
char* retrieveFromPNGFile(PNG* png);

#endif