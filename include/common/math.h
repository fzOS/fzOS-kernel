#ifndef MATH_H
#define MATH_H
#define isnormal(x) __builtin_isnormal((float)(x))
#define isfinite(x) __builtin_isfinite((float)(x))
#define isinf(x) __builtin_isinf((float)(x))
#define isnan(x) __builtin_isnan((float)(x))
#endif
