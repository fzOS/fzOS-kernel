//内核中会用到的常用类型定义。
#ifndef TYPES_H
#define TYPES_H


typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;

typedef long long I64;
typedef int I32;
typedef short I16;
typedef char I8;

typedef U8 byte;

#ifndef NULL
#define NULL 0
#endif
#define nullptr (void*) 0

#define KERNEL_ADDR_OFFSET (0xFFFF800000000000ULL)
#define KERNEL_STACK_PAGES 4

#define PAGE_SIZE 4096

#define PATH_SEPARATOR '/'
#define LINE_SEPARATOR '\n'

typedef enum {
    FzOS_ERROR=-1,
    FzOS_BUFFER_TOO_SMALL=-2,
    FzOS_DEVICE_NOT_READY=-3,
    FzOS_POSITION_OVERFLOW=-4,
    FzOS_FILE_NOT_FOUND=-5,
    FzOS_NOT_IMPLEMENTED=-6,
    FzOS_NO_SPACE_LEFT=-7,
    FzOS_INVALID_INPUT=-8,
    FzOS_READ_ONLY=-9,
    FzOS_SUCCESS=0,
} FzOSResult;

typedef union {
    U64 raw;
    U32 dword[2];
    U16 word[4];
    U8 byte[8];
}ValSplitter;

typedef struct {
    U32 first;
    U16 second;
    U16 third;
    U8 fourth[2];
    U8 fifth[6];
} __attribute__((packed)) GUID;


#endif
