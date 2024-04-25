#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// led D6 MOSI et SCK => sur le schema MOSI PB3 et et SCK = PB5

/*SCK PORTB/ bit 5
When the SPI is enabled as a Master, the data
direction of this pin is controlled by DDB5. When the pin is forced by the SPI to be an input, the pull-up can still
be controlled by the PORTB5 bit

MISO PORTB bit 4
When the SPI is enabled as a Master, this pin
is configured as an input regardless of the setting of DDB4.


SS PORTB bit 2
When the SPI is enabled as a
Master, the data direction of this pin is controlled by DDB2. When the pin is forced by the SPI to be an input, the
pull-up can still be controlled by the PORTB2 bit.
OC1B, Output Compare Match output: The PB2 pin can serve as an external output for the Timer/Counter1
Compare Match B. The PB2 pin has to be configured as an output (DDB2 set (one)) to serve this function. The
OC1B pin is also the output pin for the PWM mode timer function.
PCINT2: Pin Change Interrupt source 2. The PB2 pin can serve as an external interrupt source.
*/

#define DD_MOSI PB3 // Master Output, Slave Input (généré par le maître).
#define DD_SCK PB5	// Serial Clock, Horloge (généré par le maître).
#define DD_SS PB2	// Slave Select, Actif à l'état bas (généré par le maître). SPI Bus Master Slave select

#define DDR_SPI DDRB

// #define pin_MISO PB4 // output Master Input, Slave Output (généré par l'esclave).

#define UBRR 8 // Normal mode p165

typedef struct s_RGB
{
	uint8_t g;
	uint8_t r;
	uint8_t b;
} t_RGB;

//-------------------------------------------------------------------------------

void uart_init()
{
	// configurer le baud rate //p149
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t)UBRR;

	// activer le recepteur et transmetteur
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	// formats de donnees (8bits, pas de parite, 1 bit de stop)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_tx(char c)
{
	// attendre que le transmit buffer soit vide
	while (!(UCSR0A & (1 << UDRE0)))
	{
	}
	// data dans le buffer, envoi du buffer
	UDR0 = c;
}

void print_hexa(uint8_t num)
{
	char hex_digits[] = "0123456789ABCDEF";
	uart_tx(hex_digits[num >> 4]);	 // Envoie le chiffre des quatre bits de poids fort
	uart_tx(hex_digits[num & 0x0F]); // Envoie le chiffre des quatre bits de poids faible
}

void uart_print(const char *str)
{
	while (*str)
	{
		uart_tx(*str++);
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++*/

/*------------------------------------------*/

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

void set_leds(t_RGB *led_array, uint16_t nb_leds, uint8_t bright)
{
	bright = bright & 0x1F; // brightness en  5 bits

	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);
	SPI_MasterTransmit(0x00);

	for (uint16_t i = 0; i < nb_leds; ++i)
	{
		SPI_MasterTransmit(0xE0 | bright);	// Brithness
		SPI_MasterTransmit(led_array[i].b); // Blue
		SPI_MasterTransmit(led_array[i].g); // Green
		SPI_MasterTransmit(led_array[i].r); // Red
	}

	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
}

int main()
{
	t_RGB leds[3];

	uart_init();
	SPI_masterInit();

	leds[0].b = 0x00;
	leds[0].g = 0x00;
	leds[0].r = 0x08;

	leds[1].b = 0x00;
	leds[1].g = 0x00;
	leds[1].r = 0x00;

	leds[2].b = 0x00;
	leds[2].g = 0x00;
	leds[2].r = 0x00;

	set_leds(leds, 3, 5);

	// led? D6 C14 a quelle addresse sont ces 3 leds?
	while (1)
	{
	}
	return 0;
	// SPI_MasterTransmit();
}
