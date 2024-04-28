#ifndef MODE42_H
#define MODE42_H

#include <avr/io.h>
#include <util/twi.h>
#include <stdbool.h>

#define RED_LED_PIN PD5
#define GREEN_LED_PIN PD6
#define BLUE_LED_PIN PD3

#define DDR_SPI DDRB

#define DD_MOSI PB3 // Master Output, Slave Input (généré par le maître).
#define DD_SCK PB5	// Serial Clock, Horloge (généré par le maître).
#define DD_SS PB2	// Slave Select, Actif à l'état bas (généré par le maître). SPI Bus Master Slave select

void init_D5();

void SPI_masterInit(void);
void SPI_MasterTransmit(char cData);

void spi_start();
void spi_stop();
void turn_on_led_32(uint32_t color);
void turn_on_3_leds(uint32_t led1, uint32_t led2, uint32_t led3);

// uint16_t mode4();

#endif