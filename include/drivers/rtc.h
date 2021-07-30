#ifndef RTC_H
#define RTC_H

#define CMOS_REG  0x70
#define CMOS_DATA 0x71

#include <types.h>

typedef struct {
    U8 second;
    U8 minute;
    U8 hour;
    U8 day;
    U8 month;
    U16 year;
    U8 century;
}RTCTime;
void read_rtc(RTCTime* time);
#endif
