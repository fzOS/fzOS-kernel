#ifndef CLASS_H
#define CLASS_H
#include <types.h>
typedef struct {
    U8 type;
    U64 padding;
} __attribute__((packed)) constant_entry;
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
    U64 interface_pool_entry_offset;
    U64 fields_pool_entry_offset;
    U16 constant_pool_entry_count;
    U16 method_pool_entry_count;
    U16 interface_pool_entry_count;
    U16 fields_pool_entry_count;
    U16 access_flag,this_class,super_class;
    U64 buffer_size;
    U8 buffer[0];
} class;
typedef enum {
    CONSTANT_NULL=0,
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
    CONSTANT_NAMEANDTYPE=12,
    CONSTANT_METHODHANDLE=15,
    CONSTANT_METHODTYPE=16,
    CONSTANT_INVOKEDYNAMIC=18
} ClassConstantType;
typedef struct {
    U8 type;
    U16 name_index;
    U8 padding[sizeof(constant_entry)-1-2];
}__attribute__((packed)) ClassInfoConstant;
typedef struct {
    U8 type;
    U16 class_index;
    U16 name_and_type_index;
    U8 padding[sizeof(constant_entry)-1-2-2];
}__attribute__((packed)) FieldRefConstant,MethodRefConstant,InterfaceRefConstant;
typedef struct {
    U8 type;
    U16 string_index;
    U8 padding[sizeof(constant_entry)-1-2];
}__attribute__((packed)) StringInfoConstant;
typedef struct {
    U8 type;
    I32 val;
    U8 padding[sizeof(constant_entry)-1-4];
}__attribute__((packed)) IntegerInfoConstant;
typedef struct {
    U8 type;
    float val;
    U8 padding[sizeof(constant_entry)-1-4];
}__attribute__((packed)) FloatInfoConstant;
typedef struct {
    U8 type;
    I64 val;
    U8 padding[sizeof(constant_entry)-1-8];
}__attribute__((packed)) LongInfoConstant;
typedef struct {
    U8 type;
    double val;
    U8 padding[sizeof(constant_entry)-1-8];
}__attribute__((packed)) DoubleInfoConstant;
typedef struct {
    U8 type;
    U16 name_index;
    U16 descriptor_index;
    U8 padding[sizeof(constant_entry)-1-2-2];
}__attribute__((packed)) NameAndTypeInfoConstant;
typedef struct {
    U8 type;
    U8 reference_kind;
    U16 reference_index;
    U8 padding[sizeof(constant_entry)-1-1-2];
}__attribute__((packed)) MethodHandleInfoConstant;
typedef struct {
    U8 type;
    U16 length;
    U64 buffer_offset:48;
}__attribute__((packed)) UTF8InfoConstant;
typedef struct {
    U8 type;
    U16 descriptor_index;
    U8 padding[sizeof(constant_entry)-1-2];
}__attribute__((packed)) MethodTypeInfoConstant;
typedef struct {
    U8 type;
    U16 bootstrap_method_attr_index;
    U16 name_and_type_index;
    U8 padding[sizeof(constant_entry)-1-2-2];
}__attribute__((packed)) InvokeDynamicInfoConstant;
//防止自己翻车/在不兼容的编译器平台上构建，导致运行时异常。
_Static_assert(sizeof(constant_entry)==sizeof(ClassInfoConstant)
             &&sizeof(ClassInfoConstant)==sizeof(FieldRefConstant)
             &&sizeof(FieldRefConstant)==sizeof(StringInfoConstant)
             &&sizeof(StringInfoConstant)==sizeof(IntegerInfoConstant)
             &&sizeof(IntegerInfoConstant)==sizeof(FloatInfoConstant)
             &&sizeof(FloatInfoConstant)==sizeof(LongInfoConstant)
             &&sizeof(LongInfoConstant)==sizeof(DoubleInfoConstant)
             &&sizeof(DoubleInfoConstant)==sizeof(NameAndTypeInfoConstant)
             &&sizeof(NameAndTypeInfoConstant)==sizeof(MethodHandleInfoConstant)
             &&sizeof(MethodHandleInfoConstant)==sizeof(UTF8InfoConstant)
             &&sizeof(UTF8InfoConstant)==sizeof(MethodTypeInfoConstant)
             &&sizeof(MethodTypeInfoConstant)==sizeof(InvokeDynamicInfoConstant)
             ,"Constant Info size mismatch");
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
typedef struct {
    U16 access_flags;
    U16 name_index;
    U16 descriptor_index;
    U16 attribute_count;
    U64 attribute_info_entry_offset;
} field_info_entry,method_info_entry;
typedef struct {
    U16 attribute_name_index;
    U16 attribute_length;
    U64 info_offset;
} attribute_info_entry;
void print_class_constants(const class* c);
void print_class_info(const class* c);
const U8* class_get_utf8_string(const class* c,int no);
const U16 class_get_class_name_index(const class* c,int no);
void print_field_and_method_info(const class* c);
#endif
