#ifndef MODE_H
#define MODE_H

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

void init_ADC(uint8_t volt);
uint16_t read_ADC(uint8_t channel);

void plot_nbr_4(uint16_t n);
uint16_t mode0();
uint16_t mode1();
uint16_t mode2();
uint16_t mode4();

void mode3(void);
void mode6(void);
void mode7(void);
void mode8(void);

#endif