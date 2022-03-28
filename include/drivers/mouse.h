#ifndef MOUSE_H
#define MOUSE_H
void init_mouse(void);
void mouse_getmove(int);
typedef union {
    struct {
        int button_left:1;
        int button_right:1;
        int button_middle:1;
        int always_one:1;
        int x_offset_bit:1;
        int y_offset_bit:1;
        int x_offset_overflow:1;
        int y_offset_overflow:1;
        U8  x_offset;
        U8  y_offset;
    } __attribute__((packed)) data;
    U8 raw[3];
} MouseDataPackage;
#endif
