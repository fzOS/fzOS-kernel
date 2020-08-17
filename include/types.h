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


#endif