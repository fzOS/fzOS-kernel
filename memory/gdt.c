#include <gdt.h>
#include <kstring.h>
#include <printk.h>
#include <types.h>
struct gdt_entry {
    U16 limit15_0;
    U16 base15_0;
    U8 base23_16;
    U8 type;
    U8 limit19_16_and_flags;
    U8 base31_24;
} __attribute__((packed));

struct tss {
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
} __attribute__((packed)) tss;

__attribute__((aligned(4096))) struct {
    struct gdt_entry null;
    struct gdt_entry kernel_code;
    struct gdt_entry kernel_data;
    struct gdt_entry null2;
    struct gdt_entry user_data;
    struct gdt_entry user_code;
    struct gdt_entry tss_low;
    struct gdt_entry tss_high;
} gdt_table = {
    { 0, 0, 0, 0x00, 0x00, 0 }, /* 0x00 null  */
    { 0xff, 0, 0, 0x9a, 0xaf, 0 }, /* 0x08 kernel code (kernel base selector) */
    { 0xff, 0, 0, 0x92, 0xaf, 0 }, /* 0x10 kernel data */
    { 0, 0, 0, 0x00, 0x00, 0 }, /* 0x18 null (user base selector) */
    { 0xff, 0, 0, 0xf2, 0xaf, 0 }, /* 0x20 user data */
    { 0xff, 0, 0, 0xfa, 0xaf, 0 }, /* 0x28 user code */
    { 0, 0, 0, 0x89, 0xa0, 0 }, /* 0x30 tss low */
    { 0, 0, 0, 0x00, 0x00, 0 }, /* 0x38 tss high */
};

struct table_ptr {
    U16 limit;
    U64 base;
} __attribute__((packed));

extern void gdt_flush(struct table_ptr* gdt_ptr); //在汇编中

struct table_ptr gdt_ptr = { sizeof(gdt_table) - 1, (U64)&gdt_table };

void __attribute__ ((noinline))  gdt_flush_c(struct table_ptr* gdt_ptr)
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
    memset((void*)&tss, 0x00, sizeof(tss));
    U64 tss_base = ((U64)&tss)&~(KERNEL_ADDR_OFFSET);
    gdt_table.tss_low.base15_0 = tss_base & 0xffff;
    gdt_table.tss_low.base23_16 = (tss_base >> 16) & 0xff;
    gdt_table.tss_low.base31_24 = (tss_base >> 24) & 0xff;
    gdt_table.tss_low.limit15_0 = sizeof(tss);
    gdt_table.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;
    debug(" Loading GDT...\n");
    gdt_flush_c(&gdt_ptr);
}
