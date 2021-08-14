#ifndef BSWAP_H
#define BSWAP_H
#include <types.h>
__attribute__ ((always_inline)) inline float bswap32f(float in_float)
{
    U32* bytes = (U32*)(&in_float);
    *bytes = __builtin_bswap32(*bytes);
    return in_float;
}
__attribute__ ((always_inline)) inline double bswap64f(double in_float)
{
    U64* bytes=(U64*)(&in_float);
    *bytes = __builtin_bswap64(*bytes);
    return in_float;
}

__attribute__ ((always_inline)) inline U16 bswap16(U16 in)
{
    return __builtin_bswap16(in);
}
__attribute__ ((always_inline)) inline U32 bswap32(U32 in)
{
    return __builtin_bswap32(in);
}
__attribute__ ((always_inline)) inline U64 bswap64(U64 in)
{
    return __builtin_bswap64(in);
}
#endif
