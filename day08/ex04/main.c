#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define RED_LED_PIN PD5
#define GREEN_LED_PIN PD6
#define BLUE_LED_PIN PD3

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

t_RGB leds[3];
static uint8_t wheelON = 0;
static uint8_t position = 0; // Maintain position across calls

void uart_init()
{
	// configurer le baud rate //p149
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t)UBRR;

	// activer le recepteur et transmetteur
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

	// formats de donnees (8bits, pas de parite, 1 bit de stop)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

char uart_rx(void)
{
	while (!(UCSR0A & (1 << RXC0)))
		; // RXCO sera set quand il y aura de la data nonlue dans UDR0

	return UDR0;
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

unsigned char *avr_itoa(int i)
{
	static char str[8];
	char length = (i <= 0);
	int j = i;
	while (j)
	{
		j /= 10;
		length++;
	}
	str[length--] = '\0';
	str[0] = i < 0 ? '-' : '0';
	i *= (i > 0) - (i < 0);
	while (i)
	{
		str[length--] = i % 10 + '0';
		i /= 10;
	}
	return str;
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
/*************************************************************************/

#define BACKSPACE 0x7F
#define ENTER 0x0D

uint8_t hex_char_to_uint(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return 10 + (c - 'A');
	if (c >= 'a' && c <= 'f')
		return 10 + (c - 'a');
	return 0; // This shouldn't happen if input is validated correctly
}

void set_leds(t_RGB *led_array, uint16_t nb_leds, uint8_t bright);
void wheel(uint8_t led_index, uint8_t pos);

void set_rgb(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue)
{
	if (led_index < 3)
	{ // Assuming there are 3 LEDs
		leds[led_index] = (t_RGB){green, red, blue};
	}
	// Update the LEDs if necessary, or do it in a batch outside this function
}

void apply_rainbow(uint8_t start_pos)
{
	// for (uint8_t i = 0; i < 3; i++)
	// {											  // Assume 3 LEDs
	// 	uint8_t pos = (start_pos + i * 85) % 256; // Spread the colors across the LEDs
	// 	wheel(i, pos);
	// }
	for (uint8_t i = 0; i < 3; i++)
		wheel(i, start_pos);
	set_leds(leds, 3, 1); // Update all LEDs at once
}

void wheel(uint8_t led_index, uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
	{
		set_rgb(led_index, 255 - pos * 3, 0, pos * 3);
	}
	else if (pos < 170)
	{
		pos -= 85;
		set_rgb(led_index, 0, pos * 3, 255 - pos * 3);
	}
	else
	{
		pos -= 170;
		set_rgb(led_index, pos * 3, 255 - pos * 3, 0);
	}
}

void activate_rainbow_mode()
{

	// static uint8_t position = 0; // Maintain position across calls
	apply_rainbow(position++);
	if (position >= 256)
	{ // Reset after full cycle
		position = 0;
	}
	// _delay_ms(100); // Maintain delay for visibility

	// static uint8_t position = 0; // Start position for the rainbow
	// while (1)
	// { // Keep cycling through the rainbow colors
	// 	apply_rainbow(position++);
	// 	_delay_ms(100); // Delay for visibility, adjust as needed
	// 	if (wheelON != 1)
	// 		break; // Exit after one full cycle if needed
	// }
}

ISR(USART_RX_vect)
{
	char buffer[9] = {0}; // To store incoming hex values
	uint8_t i = 0;

	char str[50];
	char c;
	int y = 0;

	while ((c = uart_rx()) != ENTER && y < 49)
	{
		if (c == BACKSPACE && y > 0)
		{
			uart_tx('\b'); // Envoie le caractère backspace au terminal pour reculer le curseur
			uart_tx(' ');  // Remplace le dernier caractère par un espace
			uart_tx('\b'); // Recule à nouveau le curseur
			i--;		   // Décrémente l'indice pour effacer le caractère du buffer
			str[i] = '\0'; // Termine la chaîne plus tôt pour ignorer le dernier caractère
		}
		else if (c >= ' ' && c <= '~')
		{
			str[y++] = c;
			uart_tx(c);
		}
	}
	str[y] = '\0';
	uart_tx('\r');
	uart_tx('\n');

	if (strncmp(str, "#FULLRAINBOW", 12) == 0)
	{
		wheelON = 1; // Activate the rainbow mode if matched
					 // Exit the function after activating the mode
	}
	else
	{
		char *pos = strchr(str, '#'); // Find the position of '#'
		if (pos && strlen(pos) >= 9)
		{ // Make sure there are enough characters after '#'
			leds[0] = (t_RGB){0x00, 0x00, 0x00};
			leds[1] = (t_RGB){0x00, 0x00, 0x00};
			leds[2] = (t_RGB){0x00, 0x00, 0x00};

			// Extracting the RGB values from the string
			uint8_t red = hex_char_to_uint(pos[1]) * 16 + hex_char_to_uint(pos[2]);
			uint8_t green = hex_char_to_uint(pos[3]) * 16 + hex_char_to_uint(pos[4]);
			uint8_t blue = hex_char_to_uint(pos[5]) * 16 + hex_char_to_uint(pos[6]);
			char led = pos[8] - '6'; // Converting character index to integer index

			// Print the RGB values for debugging
			char buffer[4];
			itoa(red, buffer, 10);
			uart_print(buffer);
			uart_print(" ");
			itoa(green, buffer, 10);
			uart_print(buffer);
			uart_print(" ");
			itoa(blue, buffer, 10);
			uart_print(buffer);
			uart_print("\r\n");
			uart_tx(pos[8]);
			uart_print("\r\n");
			uart_tx('0' + led);
			uart_print("\r\n");

			// Setting the RGB color of the specified LED
			if (led >= 0 && led < 3)
			{ // Validate led index
				leds[led] = (t_RGB){green, red, blue};
				wheelON = 2;
				// set_leds(leds, 3, 1); // Assuming set_leds function is defined to handle the array update
			}
			else
				uart_print("Invalid number of LED!\r\n");
		}
		else
		{
			uart_print("Invalid color format!\r\n");
		}
	}
	uart_print("Enter an HEX RGB color in the format #RRGGBBDx or #FULLRAINBOW: ");
}

// ISR(USART_RX_vect)
// {
// 	char buffer[9] = {0}; // To store incoming hex values
// 	uint8_t i = 0;
// 	// char c;
// 	char user_input[50];

// 	uart_print("Enter an HEX RGB color in the format #RRGGBBDx or #FULLRAINBOW: ");
// 	get_string(user_input);

// 	if (strncmp(user_input, "#FULLRAINBOW", 12) == 0)
// 	{
// 		wheelON = 1; // Activate the rainbow mode if matched
// 					 // Exit the function after activating the mode
// 	}
// 	else
// 	{
// 		char *pos = strchr(user_input, '#'); // Find the position of '#'
// 		if (pos && strlen(pos) >= 9)
// 		{ // Make sure there are enough characters after '#'
// 			leds[0] = (t_RGB){0x00, 0x00, 0x00};
// 			leds[1] = (t_RGB){0x00, 0x00, 0x00};
// 			leds[2] = (t_RGB){0x00, 0x00, 0x00};

// 			// Extracting the RGB values from the string
// 			uint8_t red = hex_char_to_uint(pos[1]) * 16 + hex_char_to_uint(pos[2]);
// 			uint8_t green = hex_char_to_uint(pos[3]) * 16 + hex_char_to_uint(pos[4]);
// 			uint8_t blue = hex_char_to_uint(pos[5]) * 16 + hex_char_to_uint(pos[6]);
// 			char led = pos[8] - '56'; // Converting character index to integer index

// 			// Print the RGB values for debugging
// 			char buffer[4];
// 			itoa(red, buffer, 10);
// 			uart_print(buffer);
// 			uart_print(" ");
// 			itoa(green, buffer, 10);
// 			uart_print(buffer);
// 			uart_print(" ");
// 			itoa(blue, buffer, 10);
// 			uart_print(buffer);
// 			uart_print("\r\n");
// 			uart_tx('0' + led);
// 			uart_print("\r\n");

// 			// Setting the RGB color of the specified LED
// 			if (led >= 0 && led < 3)
// 			{ // Validate led index
// 				leds[led] = (t_RGB){green, red, blue};
// 				wheelON = 2;
// 				// set_leds(leds, 3, 1); // Assuming set_leds function is defined to handle the array update
// 			}
// 			else
// 				uart_print("Invalid number of LED!\r\n");
// 		}
// 		else
// 		{
// 			uart_print("Invalid color format!\r\n");
// 		}
// 	}
// }

/************************************************************/

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
	// t_RGB leds[3];

	// init_rgb();
	uart_init();
	SPI_masterInit();
	sei();
	uart_print("Enter an HEX RGB color in the format #RRGGBBDx or #FULLRAINBOW: ");
	// led? D6 C14 a quelle addresse sont ces 3 leds?
	while (1)
	{
		// handle_rgb_input();
		switch (wheelON)
		{
		case 1:
			activate_rainbow_mode(); // Update rainbow if mode is active
			break;
		case 2:
			set_leds(leds, 3, 1); // Set specific LED colors
			wheelON = 0;		  // Reset to 0 to stop updating until next input
			break;
		default:
			break; // Do nothing if wheelON is not set
		}

		_delay_ms(20);
	}
	return 0;
}
