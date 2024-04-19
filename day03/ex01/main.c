#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf#page=114&zoom=100,45,68

/*L'UART (Universal Asynchronous Receiver/Transmitter) est une
forme de communication série utilisée par les microcontrôleurs
pour échanger des données avec d'autres microcontrôleurs ou périphériques comme des ordinateurs*/

/*Voici les éléments clés de l'UART à comprendre :

	Asynchrone : Signifie que la communication ne nécessite pas de signal d'horloge entre l'émetteur
	et le récepteur pour synchroniser les données. Chaque côté doit être
	configuré avec le même baud rate (nombre de bits transmis par seconde) et format de données.
	Configuration : Typiquement, la configuration inclut le baud rate, le nombre de bits de données (souvent 8),
	le bit de parité (optionnel), et le nombre de bits de stop (généralement 1 ou 2).
	Baud rate : Doit être précisément défini des deux côtés pour une communication correcte.
	Pour nous 115200 bps.*/

// https://jazzy-shoulder-e60.notion.site/UART-908d6767aa3f4a21b3e4cdf80eefb4a8

// UBRR = USART Baud Rate Register value
// UBRR = (fcpu/16 * baud rate) -1

// On veut configuer UART pour communiquer a 115200 bauds avecune config a 8N1 (8 bits de donnees, pas de parite,1bit de stop)

// #define UBRR ((F_CPU / (16 * 115200)) - 1) // Asynchronous Normal mode(U2Xn = 0) p146/294
//  #define UBRR ((F_CPU / (8 * 115200)) - 1) // Asynchronous double speed mode(U2Xn = 1) p146/294 = +-8
#define UBRR 8 // Normal mode p165

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

void uart_printstr(const char *str)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;

	uint16_t i = 0;
	while (str[i])
	{
		uart_tx(str[i]);
		i++;
	}
}

void timer1_init()
{
	// Configurer Timer1 avec le CTC mode
	TCCR1B |= (1 << WGM12);
	// On veut une periode avec 1024 prescaler
	// Timer frequency = F_CPU / Prescaler / (1 + OCR1A)
	OCR1A = 15624 * 2;					 // (16MHz / 1024 * 1s) - 1
	TIMSK1 |= (1 << OCIE1A);			 // Enable Timer1 compare match A interrupt
	TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
}

ISR(TIMER1_COMPA_vect)
{
	uart_printstr("Hello World!\r\n");
}

int main(void)
{
	uart_init();
	timer1_init();
	sei();

	while (1)
	{
	}

	return 0;
}
