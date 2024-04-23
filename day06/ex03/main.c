#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define DEBOUNCE_TIME 50
#define UBRR 8 // Normal mode p165
#define BAUD 115200

#define RED_LED_PIN PD5
#define GREEN_LED_PIN PD6
#define BLUE_LED_PIN PD3

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

char uart_rx(void)
{
	while (!(UCSR0A & (1 << RXC0)))
		; // RXCO sera set quand il y aura de la data nonlue dans UDR0

	return UDR0;
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

/*-------------------------------------------*/
/*-------------------------------------------*/

void init_rgb()
{
	DDRD |= (1 << RED_LED_PIN) | (1 << GREEN_LED_PIN) | (1 << BLUE_LED_PIN);

	// timers pour le mode Fast PWM
	// Par exemple, pour le Timer0 (associé à PD5 et PD6 pour OC0A et OC0B respectivement)
	/*Timer0 en mode Fast PWM, ce qui signifie que le compteur du timer
	s'incrémente jusqu'à atteindre la valeur maximale (0xFF ou 255 pour un timer 8 bits), puis recommence à 0.*/
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // Mode Fast PWM

	/*Configure les sorties OC0A (PD5) et OC0B (PD6) pour qu'elles passent à 0
	lorsqu'une correspondance est trouvée entre TCNT0 (la valeur du timer) et
	OCR0A/OCR0B, et qu'elles repassent à 1 lorsque le timer repasse à 0*/
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // Clear OC0A/OC0B on Compare Match, set OC0A/OC0B at BOTTOM
	// TCCR0B |= (1 << CS01);					 // Prescaler de 8, peut-être ajusté pour une fréquence différente
	TCCR0B |= (1 << CS00);

	// Timer 2 (pour PD3 - OC2B)
	TCCR2A |= (1 << WGM21) | (1 << WGM20); // Mode Fast PWM
	TCCR2A |= (1 << COM2B1);			   // Clear OC2B on Compare Match, set at BOTTOM (non-inverting mode)
	// TCCR2B |= (1 << CS21);				   // Prescaler de 8, ajuster pour la fréquence désirée
	TCCR2B |= (1 << CS20);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	OCR0A = g;
	OCR0B = r;
	OCR2B = b;
}

#define BACKSPACE 0x7F
#define ENTER 0x0D

void get_string(char *str)
{
	char c;
	int i = 0;
	while ((c = uart_rx()) != ENTER && i < 49)
	{
		if (c == BACKSPACE && i > 0)
		{
			uart_tx('\b'); // Envoie le caractère backspace au terminal pour reculer le curseur
			uart_tx(' ');  // Remplace le dernier caractère par un espace
			uart_tx('\b'); // Recule à nouveau le curseur
			i--;		   // Décrémente l'indice pour effacer le caractère du buffer
			str[i] = '\0'; // Termine la chaîne plus tôt pour ignorer le dernier caractère
		}
		else if (c >= ' ' && c <= '~')
		{
			str[i++] = c;
			uart_tx(c);
		}
	}
	str[i] = '\0';
	uart_tx('\r');
	uart_tx('\n');
}

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

void handle_rgb_input()
{
	char buffer[9] = {0}; // To store incoming hex values
	uint8_t i = 0;
	char c;
	char user_input[50];

	uart_print("Entre une couleur HEX RGB au format #RRGGBB: ");
	get_string(user_input);

	// Apres je voudrais chercher dans user input la #et les 6valeurs suivantes
	//  Chercher la présence de '#' et extraire les 6 caractères suivants
	char *pos = strchr(user_input, '#'); // Trouver la position de '#'
	if (pos && strlen(pos) >= 7)		 // Assurez-vous qu'il y a assez de caractères après '#'
	{
		uint8_t red = hex_char_to_uint(pos[1]) * 16 + hex_char_to_uint(pos[2]);
		uint8_t green = hex_char_to_uint(pos[3]) * 16 + hex_char_to_uint(pos[4]);
		uint8_t blue = hex_char_to_uint(pos[5]) * 16 + hex_char_to_uint(pos[6]);
		uart_print(avr_itoa(red));
		uart_print(" ");
		uart_print(avr_itoa(green));
		uart_print(" ");
		uart_print(avr_itoa(blue));
		uart_print("\r\n");

		// Définir la couleur RGB de la LED
		set_rgb(red, green, blue);
	}
	else
	{
		uart_print("Invalid color format!\r\n");
	}
}

// for (uint8_t i = 0; i < 3; ++i) {
//                 entered_chars[2 * i + 1] -= entered_chars[2 * i + 1] < 'A' ? '0' : 'A' - 10;
//                 cols[i] = entered_chars[2 * i + 1] << 4;
//                 entered_chars[2 * i + 2] -= entered_chars[2 * i + 2] < 'A' ? '0' : 'A' - 10;
//                 cols[i] |= entered_chars[2 * i + 2];
//             }
//             set_rgb(cols[0], cols[1], cols[2]);
//         }
// Convert hex string to integers
// 	char val[3] = {0};
// 	val[0] = buffer[1];
// 	val[1] = buffer[2];
// 	uint8_t red = strtol(val, NULL, 16);
// 	val[0] = buffer[3];
// 	val[1] = buffer[4];
// 	uint8_t green = strtol(val, NULL, 16);
// 	val[0] = buffer[5];
// 	val[1] = buffer[6];
// 	uint8_t blue = strtol(val, NULL, 16);

// 	// Set RGB LED
// 	set_rgb(red, green, blue);
// }

int main(void)
{
	uart_init();
	init_rgb();

	while (1)
	{
		handle_rgb_input();
	}
}
