#ifndef CLASS_H
#define CLASS_H
#include <types.h>
typedef struct {
    U8 type;
    U64 offset;
} constant_entry;
typedef struct {
    U16 access_flags;
    U16 name_index;
    U16 desc_index;
    U16 attributes;
    void* attributes_entry;
    void* bytecode;
} method_entry;
typedef struct {
    //由于JVM的奇怪的特性，只能用静态结构+索引的方式进行数据存储了……
    U64 constant_entry_offset;
    U64 method_pool_entry_offset;
    U64 annotation_pool_entry_offset;
    U64 fileds_pool_entry_offset;
    U16 constant_pool_entry_count;
    U16 method_pool_entry_count;
    U16 annotation_pool_entry_count;
    U16 fields_pool_entry_count;
    U16 access_flag,this_class,suoer_class;
    U8 buffer[0];
} class;
typedef enum {
    CONSTANT_UTF8=1,
    CONSTANT_INT=2,
    CONSTANT_FLOAT=4,
    CONSTANT_LONG=5,
    CONSTANT_DOUBLE=6,
    CONSTANT_CLASS=7,
    CONSTANT_STRING=8,
    CONSTANT_FIELDREF=9,
    CONSTANT_METHODREF=10,
    CONSTANT_INTERFACEMETHODREF=11,
    CONSTANT_NAMEANDTYPE=12
} ClassConstantType;
typedef enum {
    ACCESS_PUBLIC=0x0001,
    ACCESS_PRIVATE=0x0002,
    ACCESS_PROTECTED=0x0004,
    ACCESS_STATIC=0x0008,
    ACCESS_FINAL=0x0010,
    ACCESS_SUPER=0x0020,
    ACCESS_BRIDGE=0x0040,
    ACCESS_VARARGS=0x0080,
    ACCESS_NATIVE=0x0100,
    ACCESS_INTERFACE=0x0200,
    ACCESS_ABSTRACT=0x0400,
    ACCESS_STRICT=0x0800,
    ACCESS_SYNTHETIC=0x1000,
    ACCESS_ANNOTATION=0x2000,
    ACCESS_ENUM=0x4000,
} ClassAccessFlag;
#endif
