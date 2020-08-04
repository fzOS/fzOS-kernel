//系统调用变量定义。
#ifndef SYSCALL
#define SYSCALL

#include <types.h>
//中断描述符定义。
typedef struct {
    U16 offset_low; // offset bits 0..15
    U16 selector; // a code segment selector in GDT or LDT
    U8 ist; // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    U8 type_attr; // type and attributes
    U16 offset_mid; // offset bits 16..31
    U32 offset_high; // offset bits 32..63
    U32 zero; // reserved
} IDTDescr;
//中断帧定义。
typedef struct {
    // RIP寄存器值
    U64 instruction_pointer;
    // CS寄存器值
    U64 code_segment;
    // 在调用处理器程序前rflags寄存器的值
    U64 rflags;
    // 中断时的堆栈指针。
    U64 stack_pointer;
    // 中断时的堆栈段描述符（在64位模式下通常为零）
    U64 stack_segment
} interrupt_frame;
//定义系统调用编号。
#define SYSCALL_OPEN 1
#define SYSCALL_READ 2
#define SYSCALL_WRITE 3
#define SYSCALL_CLOSE 4
#endif
