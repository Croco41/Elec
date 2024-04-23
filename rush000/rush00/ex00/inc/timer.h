#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

#define NULL ((void *)0)

void timer_init(void);
void wait_one_second(void (*f)(void));

#endif