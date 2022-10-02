#include <memory/gdt.h>
#include <common/kstring.h>
#include <common/printk.h>
#include <types.h>
struct GDTEntry {
    U16 limit15_0;
    U16 base15_0;
    U8 base23_16;
    U8 type;
    U8 limit19_16_and_flags;
    U8 base31_24;
} __attribute__((packed));

// 任务状态段 保存cpu的运行状态
struct TSS {
    U32 reserved0;
    U64 rsp0;
    U64 rsp1;
    U64 rsp2;
    U64 reserved1;
    U64 ist1;
    U64 ist2;
    U64 ist3;
    U64 ist4;
    U64 ist5;
    U64 ist6;
    U64 ist7;
    U64 reserved2;
    U16 reserved3;
    U16 iopb_offset;
} __attribute__((packed)) TSS;

__attribute__((aligned(4096))) struct {
    struct GDTEntry null;
    struct GDTEntry kernel_code;
    struct GDTEntry kernel_data;
    struct GDTEntry null2;
    struct GDTEntry user_data;
    struct GDTEntry user_code;
    struct GDTEntry tss_low;
    struct GDTEntry tss_high;
} GDTTable = {
    { 0, 0, 0, 0x00, 0x00, 0 }, /* 0x00 null  */
    { 0xff, 0, 0, 0x9a, 0xaf, 0 }, /* 0x08 kernel code (kernel base selector) */
    { 0xff, 0, 0, 0x92, 0xaf, 0 }, /* 0x10 kernel data */
    { 0, 0, 0, 0x00, 0x00, 0 }, /* 0x18 null (user base selector) */
    { 0xff, 0, 0, 0xf2, 0xaf, 0 }, /* 0x20 user data */
    { 0xff, 0, 0, 0xfa, 0xaf, 0 }, /* 0x28 user code */
    { 0, 0, 0, 0x89, 0xa0, 0 }, /* 0x30 tss low */
    { 0, 0, 0, 0x00, 0x00, 0 }, /* 0x38 tss high */
};

struct TablePtr {
    U16 limit;
    U64 base;
} __attribute__((packed));

struct TablePtr g_gdt_ptr = { sizeof(GDTTable) - 1, (U64)&GDTTable };

void __attribute__ ((noinline))  gdt_flush_c(struct TablePtr* gdt_ptr)
{
    __asm__ volatile(
        "movq $0x0,%%rax\n"
        "mov  %%ax,%%ds\n"
        "mov  %%ax,%%es\n"
        "mov  %%ax,%%fs\n"
        "mov  %%ax,%%gs\n"
        "lgdt (%%rdi)\n"            //Load GDT
        "mov $0x30, %%ax\n"         //
        "ltr %%ax\n"                // Load TSS
        "movq %%rsp, %%rax\n"       //
        "pushq $0x10\n"             // New SS at 16-bytes in to GDT
        "pushq %%rax\n"             // New RSP
        "pushfq\n"                  // New flags
        "pushq $0x08\n"             // New CS at 8-bytes in to GDT
        "lea 0x03(%%rip),%%rax\n"   // 人工PIE，一定不能动下面两条指令！！！！
        "pushq %%rax\n"             // New RIP
        "iretq\n"                   // Loads registers (the CS register cannot be set via mov) and jumps to 1:
        "nop\n"
        :::
    );

}
void init_gdt()
{
    memset((void*)&TSS, 0x00, sizeof(TSS));
    U64 tss_base = ((U64)&TSS)&~(KERNEL_ADDR_OFFSET);
    GDTTable.tss_low.base15_0 = tss_base & 0xffff;
    GDTTable.tss_low.base23_16 = (tss_base >> 16) & 0xff;
    GDTTable.tss_low.base31_24 = (tss_base >> 24) & 0xff;
    GDTTable.tss_low.limit15_0 = sizeof(TSS);
    GDTTable.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;
    gdt_flush_c(&g_gdt_ptr);
}
