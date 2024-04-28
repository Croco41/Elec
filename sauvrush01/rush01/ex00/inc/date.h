#ifndef DATE_H
#define DATE_H

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#define PCF8563_ADDRESS 0x51
#define REG_SECONDS 0x02
#define REG_MINUTES 0x03
#define REG_HOURS 0x04
#define REG_DAY 0x05
#define REG_MONTH 0x07
#define REG_YEAR 0x08

void read_time();
void update_time(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t month, uint8_t year);
uint8_t to_bcd(uint8_t n);
// void loop();
void mode9();
void mode10();
void mode11();

#endif