#ifndef MATH_H
#define MATH_H
#define isnormal(x)                                                      \
    ( sizeof(x) == sizeof(float)  ? __inline_isnormalf((float)(x))       \
    : sizeof(x) == sizeof(double) ? __inline_isnormald((double)(x))      \
                                  : __inline_isnormall((long double)(x)))

#define isfinite(x)                                                      \
    ( sizeof(x) == sizeof(float)  ? __inline_isfinitef((float)(x))       \
    : sizeof(x) == sizeof(double) ? __inline_isfinited((double)(x))      \
                                  : __inline_isfinitel((long double)(x)))

#define isinf(x)                                                         \
    ( sizeof(x) == sizeof(float)  ? __inline_isinff((float)(x))          \
    : sizeof(x) == sizeof(double) ? __inline_isinfd((double)(x))         \
                                  : __inline_isinfl((long double)(x)))

#define isnan(x)                                                         \
    ( sizeof(x) == sizeof(float)  ? __inline_isnanf((float)(x))          \
    : sizeof(x) == sizeof(double) ? __inline_isnand((double)(x))         \
                                  : __inline_isnanl((long double)(x)))
#endif
