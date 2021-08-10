#ifndef CHARDEV
#define CHARDEV
#include <types.h>
//字符型设备的定义。
typedef struct char_dev{
    U8(*getchar)(struct char_dev*);
    void(*putchar)(struct char_dev*,U8 data);
    void(*flush)(struct char_dev*);
} char_dev;
//就先这两个吧。
#endif
