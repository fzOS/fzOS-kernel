#include <efi.h>
#include <efilib.h>
#include <uefivars.h>
#include <types.h>

#ifndef _KERNEL_H_
#define _KERNEL_H_

typedef struct {
  void *xdsp_address;
  U8 *memory_map;
  U64 mem_map_size;
  U64 mem_map_descriptor_size;
  U64 kernel_lowest_address;
  U64 kernel_page_count;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
} KernelInfo;

typedef void (*KernelMainFunc)(KernelInfo);

void kernel_main(KernelInfo info);

#endif  // _KERNEL_H_