//内核中会用到的常用类型定义。
#ifndef TYPES_H
#define TYPES_H

//记录我们是否在debug模式下。
#define FZOS_DEBUG_SWITCH

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

#define FzOS_ERROR -1
#define FzOS_BUFFER_TOO_SMALL -2
#define FzOS_DEVICE_NOT_READY -3
#define FzOS_POSITION_OVERFLOW -4
#define FzOS_FILE_NOT_FOUND -5
#define FzOS_NOT_IMPLEMENTED -6
#define FzOS_SUCEESS 0

typedef union {
    U64 raw;
    U32 dword[2];
    U16 word[4];
    U8 byte[8];
}val_splitter;

typedef struct {
    U32 first;
    U16 second;
    U16 third;
    U8 fourth[2];
    U8 fifth[6];
} __attribute__((packed)) GUID;


#endif
