#include <common/cpuid.h>
#include <common/printk.h>
#include <types.h>
typedef union {
    struct {
        char data[4];
        U32 padding;
    } __attribute((packed)) split;
    U64 raw;
} CpuidRegisters;

static CpuidRegisters g_rax,g_rbx,g_rcx,g_rdx;
int do_cpuid(void) 
{
    __asm__(
        "movq %0,%%rax;"
        "movq %1,%%rbx;"
        "movq %2,%%rcx;"
        "movq %3,%%rdx;"
        "cpuid;"
        "movq %%rax, %0;"
        "movq %%rbx, %1;"
        "movq %%rcx, %2;"
        "movq %%rdx, %3;"
        : "=g"(g_rax.raw), "=g"(g_rbx.raw), "=g"(g_rcx.raw), "=g"(g_rdx.raw)
        : "r"(g_rax.raw), "r"(g_rbx.raw), "r"(g_rcx.raw), "r"(g_rdx.raw)
        : "%rax","%rbx","%rcx","%rdx","memory");
    return FzOS_SUCCESS;
}
int get_processor_vendor(char* buff)
{
    g_rax.raw=0;
    do_cpuid();
    //把CPUID拆解开。(Ugly)
    for(int i=0;i<4;i++)
    {
        buff[i]=g_rbx.split.data[i];
    }
    for(int i=4;i<8;i++)
    {
        buff[i]=g_rdx.split.data[i-4];
    }
    for(int i=8;i<12;i++)
    {
        buff[i]=g_rcx.split.data[i-8];
    }
    return 0;
}
int get_processor_rdseed_support(void)
{
    g_rcx.raw=0;
    g_rax.raw=7;
    do_cpuid();
    if(g_rbx.raw&(1<<18)) {
        return FzOS_SUCCESS;
    }
    return FzOS_NOT_IMPLEMENTED;
}
int get_processor_name(char* buff)
{
    for(int j=0;j<3;j++)
    {
        g_rax.raw=0x80000002+j;
        do_cpuid();
        for(int i=0;i<4;i++)
        {
            buff[i+16*j]=g_rax.split.data[i];
        }
        for(int i=4;i<8;i++)
        {
            buff[i+16*j]=g_rbx.split.data[i-4];
        }
        for(int i=8;i<12;i++)
        {
            buff[i+16*j]=g_rcx.split.data[i-8];
        }
        for(int i=12;i<16;i++)
        {
            buff[i+16*j]=g_rdx.split.data[i-12];
        }
    }
    return FzOS_SUCCESS;

}

