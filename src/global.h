#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#include <stddef.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <intrin.h>
    #include <Windows.h>
    #define byteswap _byteswap_ulong
#elif defined(__linux__)
    #include <byteswap.h>
    #define byteswap __builtin_bswap32
#endif

#define HEADER 8
#define KILO_BYTE 1024
#define BITS_IN_BYTE 8

typedef uint8_t byte;

typedef struct array_t
{
    void* data;
    size_t capacity;
    size_t size;
} Array;

void initArray(Array* array, uint8_t element_size);
void freeArray(Array* array);

// WARNING!!!: array MUST be an Array*, make sure the value is the same type as the type argument
#define append(array, type, value)      \
if ((array)->size < (array)->capacity)  \
{                                       \
    type* arr = (type*)(array)->data;   \
    arr[(array)->size] = value;         \
    (array)->size++;                    \
}

// WARNING!!!: array MUST be an Array*, make sure ptr is a type* (a pointer of the type argument)
#define get(array, type, index, ptr)    \
type* arr = (type*)(array)->data;       \
*(ptr) = arr[index];

#define APPEND(type)                                \
static void append##type(Array* array, type value)  \
{                                                   \
    if (array->size >= array->capacity)             \
    {                                               \
        array->capacity *= 2;                       \
        void* temp = realloc(array->data, array->capacity * sizeof(type));\
                                                    \
        if (temp == NULL)                           \
        {                                           \
            printf("Failed to allocate memory for array\n");\
            exit(EXIT_FAILURE);                     \
        }                                           \
                                                    \
        array->data = temp;                         \
    }                                               \
                                                    \
    type* arr = (type*)array->data;                 \
    arr[array->size] = value;                       \
    array->size++;                                  \
}

#define GET(type)                                   \
static type get##type(Array* array, int index)      \
{                                                   \
    type* arr = (type*)array->data;                 \
    return arr[index];                              \
}

#define ARRAY_METHODS(type)                         \
APPEND(type)                                        \
GET(type)

#endif