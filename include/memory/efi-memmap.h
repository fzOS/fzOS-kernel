#ifndef EFI_MEMMAP
#define EFI_MEMMAP
#include <types.h>
typedef struct {
    U64 Type; //和标准不符？？？
    U64 PhysicalStart;
    U64 VirtualStart;
    U64 NumberOfPages;
    U64 Attribute;
    U64 StrangePadding;
} memmap;
char* __attribute__((weak)) mem_types[]= {
    "EfiReservedMemoryType     ",
    "EfiLoaderCode             ",
    "EfiLoaderData             ",
    "EfiBootServicesCode       ",
    "EfiBootServicesData       ",
    "EfiRuntimeServicesCode    ",
    "EfiRuntimeServicesData    ",
    "EfiConventionalMemory     ",
    "EfiUnusableMemory         ",
    "EfiACPIReclaimMemory      ",
    "EfiACPIMemoryNVS          ",
    "EfiMemoryMappedIO         ",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode                ",
    "EfiMaxMemoryType          ",
} ;
#endif