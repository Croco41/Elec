#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdio.h> //pour sprintf

// ATmega328P EEPROM 1Kbytes
// accès à 1 Ko de mémoire EEPROM (soit 1024 octets)
// 1024 octets d’EEPROM = 1024 valeurs de 1 octet (de type byte / char / uint8_t / int8_t / boolean, par exemple)
// EECR |= (1<<EEMPE); /* start EEPROM write */
/*When the EEPROM is read, the CPU is halted for four clock cycles before the next instruction is executed.
When the EEPROM is written, the CPU is halted for two clock cycles before the next instruction is executed.*/

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
	DDRD &= ~(1 << DDD2);
	// Activer la resistance de pull-up pour le bouton
	PORTD |= (1 << PD2);
	// permet d'eviter les etats flottants
}

void display_binary(uint8_t value)
{
	PORTB =
		((value & 0x07)) |
		((value & 0x08) << 1); // Shift bit 3 into position for PB4)
}

void save_counter(uint8_t value)
{
	eeprom_write_byte((uint8_t *)0, value); // Écrire la valeur du compteur à l'adresse 0 de l'EEPROM
}

uint8_t load_counter(void)
{
	uint8_t readvalue = eeprom_read_byte((const uint8_t *)0);
	return readvalue; // Lire la valeur du compteur depuis l'adresse 0 de l'EEPROM
}

/*    La fonction « read » : pour lire 1 octet stocké dans l’eeprom, à une adresse donnée
	La fonction « write » : pour écrire 1 octet stocké dans l’eeprom, à une adresse donnée
	La fonction « update » : pour mettre à jour 1 octet présent dans l’eeprom (l’enregistrement
	de cette valeur ne se fera que si la valeur précédente est différente ; ceci permet d’éviter de
	consommer des « cycles d’écritures » inutilement, afin de préserver au maximum la durée de vie de l’EEPROM)
	La fonction « get » : pour lire tout type de variable primitive (byte, int, float, …) ou structure
	(de type « struct », donc), à partir d’une adresse donnée
	La fonction « put » : pour écrire une variable primitive (comme vu juste avant), ou de type struct,
	là encore à partir d’une adresse bien précise*/

// 1024 adresses de 0 a 1023
// donc pour enregistrer une valeur, il faut choisir une adresse

int main(void)
{
	uart_init();
	init_setup();
	uint8_t counter = 0;
	char buffer[10]; // Assez grand pour contenir n'importe quel nombre 8 bits et terminateur de chaîne

	// int adr_counter = 0;
	// // EEPROM.get(adr_counter, counter);
	uart_print("\r\n");
	uart_print("Debut utilisation compteur EEPROM\r\n");
	uint8_t buttonIncPressedLast = 0;
	if (load_counter() >= 16)
	{
		uart_print("Initialisation du compteur a 0 ");
		save_counter(counter);
	}
	else
	{
		counter = load_counter();
		display_binary(counter);
		uart_print("Valeur sauvegardee du compteur= ");
		sprintf(buffer, "%d", counter); // Convertit counter en chaîne
		uart_print(buffer);
		uart_print("\r\n");
	}

	while (1)
	{
		// Gestion du bouton d'incrémentation
		if (!(PIND & (1 << PD2)))
		{
			if (!buttonIncPressedLast)
			{
				_delay_ms(DEBOUNCE_TIME);
				if (!(PIND & (1 << PD2)))
				{
					counter++;
					if (counter == 16)
						counter = 0;
					save_counter(counter);
					display_binary(counter);
					buttonIncPressedLast = 1;

					char countValueStr[10];
					int loadedCount = load_counter();
					sprintf(countValueStr, "%d", loadedCount);
					uart_print("Current EEPROM counter value: ");
					uart_print(countValueStr);
					uart_print("\r\n");
				}
			}
		}
		else
		{
			buttonIncPressedLast = 0;
		}
	}
	return 0;
}
