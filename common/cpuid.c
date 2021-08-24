#include <common/cpuid.h>
#include <common/printk.h>
#include <types.h>
typedef union {
    struct {
        char data[4];
        U32 padding;
    } __attribute((packed)) split;
    U64 raw;
} cpuid_registers;

static cpuid_registers rax,rbx,rcx,rdx;
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
        : "=g"(rax.raw), "=g"(rbx.raw), "=g"(rcx.raw), "=g"(rdx.raw)
        : "r"(rax.raw), "r"(rbx.raw), "r"(rcx.raw), "r"(rdx.raw)
        : "%rax","%rbx","%rcx","%rdx","memory");
    return FzOS_SUCCESS;
}
int get_processor_vendor(char* buff)
{
    rax.raw=0;
    do_cpuid();
    //把CPUID拆解开。(Ugly)
    for(int i=0;i<4;i++)
    {
        buff[i]=rbx.split.data[i];
    }
    for(int i=4;i<8;i++)
    {
        buff[i]=rdx.split.data[i-4];
    }
    for(int i=8;i<12;i++)
    {
        buff[i]=rcx.split.data[i-8];
    }
    return 0;
}
int get_processor_rdseed_support(void)
{
    rcx.raw=0;
    rax.raw=7;
    do_cpuid();
    if(rbx.raw&(1<<18)) {
        return FzOS_SUCCESS;
    }
    return FzOS_NOT_IMPLEMENTED;
}
int get_processor_name(char* buff)
{
    for(int j=0;j<3;j++)
    {
        rax.raw=0x80000002+j;
        do_cpuid();
        for(int i=0;i<4;i++)
        {
            buff[i+16*j]=rax.split.data[i];
        }
        for(int i=4;i<8;i++)
        {
            buff[i+16*j]=rbx.split.data[i-4];
        }
        for(int i=8;i<12;i++)
        {
            buff[i+16*j]=rcx.split.data[i-8];
        }
        for(int i=12;i<16;i++)
        {
            buff[i+16*j]=rdx.split.data[i-12];
        }
    }
    return FzOS_SUCCESS;

}

