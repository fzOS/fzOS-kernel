#include <types.h>
#include <gdt.h>
#include <printk.h>
#include <kstring.h>
struct gdt_entry {
  U16 limit15_0;            U16 base15_0;
  U8  base23_16;            U8  type;
  U8  limit19_16_and_flags; U8  base31_24;
}__attribute__((packed));

struct tss {
    U32 reserved0; U64 rsp0;      U64 rsp1;
    U64 rsp2;      U64 reserved1; U64 ist1;
    U64 ist2;      U64 ist3;      U64 ist4;
    U64 ist5;      U64 ist6;      U64 ist7;
    U64 reserved2; U16 reserved3; U16 iopb_offset;
} __attribute__((packed)) tss;

__attribute__((aligned(4096))) 
struct {
  struct gdt_entry null;
  struct gdt_entry kernel_code;
  struct gdt_entry kernel_data;
  struct gdt_entry null2;
  struct gdt_entry user_data;
  struct gdt_entry user_code;
  struct gdt_entry tss_low;
  struct gdt_entry tss_high;
} gdt_table = {
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x00 null  */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x08 kernel code (kernel base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x10 kernel data */
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x18 null (user base selector) */
    {0, 0, 0, 0x92, 0xa0, 0},  /* 0x20 user data */
    {0, 0, 0, 0x9a, 0xa0, 0},  /* 0x28 user code */
    {0, 0, 0, 0x89, 0xa0, 0},  /* 0x40 tss low */
    {0, 0, 0, 0x00, 0x00, 0},  /* 0x48 tss high */
} ;


struct table_ptr {
    U16 limit;
    U64 base;
} __attribute__((packed));


extern void gdt_flush(struct table_ptr * gdt_ptr); //在汇编中

void * memzero(void * s, U64 n) {
    for (int i = 0; i < n; i++) ((U8*)s)[i] = 0;
    return s;
}

void init_gdt() {
    memzero((void*)&tss,sizeof(tss));
    U64 tss_base = ((U64)&tss);
    gdt_table.tss_low.base15_0 = tss_base & 0xffff;
    gdt_table.tss_low.base23_16 = (tss_base >> 16) & 0xff;
    gdt_table.tss_low.base31_24 = (tss_base >> 24) & 0xff;
    gdt_table.tss_low.limit15_0 = sizeof(tss);
    gdt_table.tss_high.limit15_0 = (tss_base >> 32) & 0xffff;
    struct table_ptr gdt_ptr = { sizeof(gdt_table)-1, (U64)&gdt_table };
    printk("loading gdt...\n");
    gdt_flush(&gdt_ptr);
}