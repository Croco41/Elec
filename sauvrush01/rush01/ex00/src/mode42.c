#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "i2c.h"
#include "expander.h"
#include "capteur.h"
#include "mode.h"
#include "timer.h"
#include "mode42.h"

volatile uint16_t count_color = 0;

void init_D5()
{
	DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD3);
}

void SPI_masterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(char cData)
{
	/*Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;
}

void spi_start()
{
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);
}

void spi_stop()
{
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
}

void turn_on_led_32(uint32_t color)
{
	SPI_MasterTransmit((color & 0xFF000000) >> 24);
	SPI_MasterTransmit((color & 0x000000FF));		// Rouge
	SPI_MasterTransmit((color & 0x0000FF00) >> 8);	// Vert
	SPI_MasterTransmit((color & 0x00FF0000) >> 16); // Bleu
}

void turn_on_3_leds(uint32_t led1, uint32_t led2, uint32_t led3)
{
	spi_start();

	turn_on_led_32(led1);
	turn_on_led_32(led2);
	turn_on_led_32(led3);

	spi_stop();
}
