#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "zlib.h"
#include "global.h"
#include "png.h"
#include "cmd.h"
#include "filter.h"
#include "bit_manipulation.h"

ARRAY_METHODS(int);
ARRAY_METHODS(Chunk);

int main(int argc, char* argv[])
{
    PNG png;
    loadPNG(&png, argv[1]);

    int width, height;
    getDimensions(&png, &width, &height);

    printf("----------\n");
    printf("%d chunks\n", png.chunks.size);
    printf("Width: %d, Height: %d\n", width, height);
    printf("Byte Per Pixel: %d\n", bytesPerPixel(png.ihdr.color_type, png.ihdr.bit_depth));

    printIHDR(&png.ihdr);

    for (int i = 0; i < png.chunks.size; ++i)
    {
        Chunk chunk = getChunk(&png.chunks, i);
        printf("[%d|%s|%lu]\n", chunk.length, chunk.type, chunk.crc);
    }

    if (argv[2][0] == 't')
    {
        const char* message = "Hello!";
        createModifiedPNGFile(&png, message); 
    }
    else if (argv[2][0] == 'f')
    {
        char* message = retrieveFromPNGFile(&png);
        printf("message: %s\n", message);
        free(message);
    }

    for (int i = 0; i < png.chunks.size; ++i)
    {
        Chunk chunk = getChunk(&png.chunks, i);
        free(chunk.data);
    }

    freeArray(&png.chunks);
    free(png.header);
}