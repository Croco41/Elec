#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h> //pour sprintf

#define DEBOUNCE_TIME 50
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

void init_setup()
{
	// Configurer les LED en sortie (pour le portB)
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);
	// Configurer les bouton en entree (port D)
	DDRD &= ~(1 << DDD2) | (1 << DDD4);
	// Activer la resistance de pull-up pour le bouton
	PORTD |= (1 << PD2) | (1 << PD4);
	// permet d'eviter les etats flottants
}

void display_binary(uint8_t value)
{
	PORTB =
		((value & 0x07)) |
		((value & 0x08) << 1); // Shift bit 3 into position for PB4)
}

void write_counter(uint8_t c_addr, uint8_t value)
{
	// Écriture de la valeur du compteur dans la mémoire EEPROM
	eeprom_write_byte((uint8_t *)c_addr, value);
}

uint8_t read_counter(uint16_t c_addr)
{
	// Lecture de la valeur du compteur depuis la mémoire EEPROM
	return eeprom_read_byte((const uint8_t *)c_addr);
}

/*---------------------------------------------------*/

int main(void)
{
	uart_init();
	init_setup();

	uint8_t valeur = 0;
	uint8_t counter = 0;
	uint8_t buttonIncPressedLast = 0;
	uint8_t buttonDecPressedLast = 0;

	for (int i = 0; i < 4; i++)
	{
		if (read_counter(i) >= 16)
			write_counter(i, 0);
	}

	while (1)
	{
		if (!(PIND & (1 << PD2)))
		{ // SW1 pressed
			if (!buttonIncPressedLast)
			{
				_delay_ms(DEBOUNCE_TIME);
				if (!(PIND & (1 << PD2)))
				{

					uint8_t counterValue = read_counter(counter);
					counterValue++;
					if (counterValue >= 16)
						counterValue = 0;

					write_counter(counter, counterValue);
					display_binary(counterValue);
					buttonIncPressedLast = 1;

					char countValueStr[10];
					int loadedCount = read_counter(counter);
					char SelectedCounterStr[10];
					sprintf(countValueStr, "%d", loadedCount);
					sprintf(SelectedCounterStr, "%d", counter);
					uart_print("Current EEPROM counter value: ");
					uart_print(countValueStr);
					uart_print(" Compteur n= ");
					uart_print(SelectedCounterStr);
					uart_print("\r\n");
				}
			}
		}
		else
		{
			buttonIncPressedLast = 0;
		}

		if (!(PIND & (1 << PD4)))
		{ // SW2 pressed
			if (!buttonDecPressedLast)
			{
				_delay_ms(DEBOUNCE_TIME);
				if (!(PIND & (1 << PD4)))
				{
					counter++;
					if (counter == 4)
						counter = 0;
					buttonDecPressedLast = 1;
				}
			}
		}
		else
		{
			buttonDecPressedLast = 0;
		}
	}

	return 0;
}
