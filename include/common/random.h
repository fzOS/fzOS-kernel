#ifndef RANDOM_H
#define RANDOM_H
#include <types.h>
int init_random(void);
U8 random_get_u8(void);
U16 random_get_u16(void);
U32 random_get_u32(void);
U64 random_get_u64(void);
#endif
