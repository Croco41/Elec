#ifndef UARTDATE_H
#define UARTDATE_H

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#define BACKSPACE 0x7F
#define ENTER 0x0D

void uarttimesw3();
int validate_date_time(const char *str);

#endif