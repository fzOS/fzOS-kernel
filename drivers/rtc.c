#include <drivers/rtc.h>
#include <common/io.h>
#ifndef THIS_YEAR
#define THIS_YEAR 2021
#endif
int get_update_in_progress_flag() {
    outb(CMOS_REG, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

unsigned char get_RTC_register(int reg) {
    outb(CMOS_REG, reg);
    return inb(CMOS_DATA);
}

void read_rtc(RTCTime* time) {
    U8 last_second;
    U8 last_minute;
    U8 last_hour;
    U8 last_day;
    U8 last_month;
    U8 last_year;
    U8 last_century;
    U8 registerB;
    while (get_update_in_progress_flag());
    time->second = get_RTC_register(0x00);
    time->minute = get_RTC_register(0x02);
    time->hour = get_RTC_register(0x04);
    time->day = get_RTC_register(0x07);
    time->month = get_RTC_register(0x08);
    time->year = get_RTC_register(0x09);
    do {
        last_second = time->second;
        last_minute = time->minute;
        last_hour = time->hour;
        last_day = time->day;
        last_month = time->month;
        last_year = time->year;
        last_century = time->century;
        while (get_update_in_progress_flag());
        time->second = get_RTC_register(0x00);
        time->minute = get_RTC_register(0x02);
        time->hour = get_RTC_register(0x04);
        time->day = get_RTC_register(0x07);
        time->month = get_RTC_register(0x08);
        time->year = get_RTC_register(0x09);
      }
    while((last_second != time->second) || (last_minute != time->minute) || (last_hour != time->hour) ||
            (last_day != time->day) || (last_month != time->month) || (last_year != time->year) ||
            (last_century != time->century));
    registerB = get_RTC_register(0x0B);
    // Convert BCD to binary values if necessary
    if (!(registerB & 0x04)) {
        time->second = (time->second & 0x0F) + ((time->second / 16) * 10);
        time->minute = (time->minute & 0x0F) + ((time->minute / 16) * 10);
        time->hour = ((time->hour & 0x0F) + (((time->hour & 0x70) / 16) * 10) ) | (time->hour & 0x80);
        time->day = (time->day & 0x0F) + ((time->day / 16) * 10);
        time->month = (time->month & 0x0F) + ((time->month / 16) * 10);
        time->year = (time->year & 0x0F) + ((time->year / 16) * 10);
    }
    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(registerB & 0x02) && (time->hour & 0x80)) {
        time->hour = ((time->hour & 0x7F) + 12) % 24;
    }
    time->century = THIS_YEAR / 100;
    // Calculate the full (4-digit) year
    time->year += (THIS_YEAR / 100) * 100;
    if(time->year < THIS_YEAR ) time->year += 100;
}
