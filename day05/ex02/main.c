#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdbool.h>
#include <stdint.h>

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

// void write_counter(uint8_t c_addr, uint8_t value)
// {
// 	// Écriture de la valeur du compteur dans la mémoire EEPROM
// 	eeprom_write_byte((uint8_t *)c_addr, value);
// }

// uint8_t read_counter(uint16_t c_addr)
// {
// 	// Lecture de la valeur du compteur depuis la mémoire EEPROM
// 	return eeprom_read_byte((const uint8_t *)c_addr);
// }

#define EEPROM_SIZE 1024
#define MAGIC_NUMBER 0xA5
#define MAGIC_OFFSET 0
#define DATA_START (MAGIC_OFFSET + sizeof(uint8_t))

// Fonction pour écrire de manière sécurisée dans l'EEPROM
bool safe_eeprom_write(void *buffer, size_t offset, size_t length)
{

	// Vérifier que les données sont dans les limites de l'EEPROM
	if (DATA_START + offset + length > EEPROM_SIZE)
	{
		uart_print("Erreur : tentative d'écriture hors limites de l'EEPROM.\n");
		return false; // Retourner false si la tentative d'écriture est hors limites
	}
	uint8_t magic = eeprom_read_byte((uint8_t *)MAGIC_OFFSET);
	if (magic != MAGIC_NUMBER)
	{
		// Si le nombre magique n'est pas défini, définissez-le et écrivez les données
		uart_print("Initialisation du nombre magique et écriture des données.\n");
		eeprom_write_byte((uint8_t *)MAGIC_OFFSET, MAGIC_NUMBER);
		for (size_t i = 0; i < length; i++)
		{
			eeprom_write_byte((uint8_t *)(DATA_START + offset + i), ((uint8_t *)buffer)[i]);
		}
		return true;
	}
	else
	{
		// Vérifiez si les données doivent être réécrites
		bool need_rewrite = false;
		for (size_t i = 0; i < length; i++)
		{
			if (eeprom_read_byte((uint8_t *)(DATA_START + offset + i)) != ((uint8_t *)buffer)[i])
			{
				need_rewrite = true;
				break;
			}
		}
		if (need_rewrite)
		{
			uart_print("Réécriture nécessaire, mise à jour des données.\n");
			for (size_t i = 0; i < length; i++)
			{
				eeprom_write_byte((uint8_t *)(DATA_START + offset + i), ((uint8_t *)buffer)[i]);
			}
		}
		else
		{
			uart_print("Aucune réécriture nécessaire, les données sont déjà à jour.\n");
		}
		return need_rewrite;
	}
}

// Fonction pour lire de manière sécurisée depuis l'EEPROM
bool safe_eeprom_read(void *buffer, size_t offset, size_t length)
{
	uint8_t magic = eeprom_read_byte((uint8_t *)MAGIC_OFFSET);
	if (magic == MAGIC_NUMBER)
	{
		// Si le nombre magique est correct, lire les données
		uart_print("REading to EEPROM, nb Magic ok.\n");
		for (size_t i = 0; i < length; i++)
		{
			((uint8_t *)buffer)[i] = eeprom_read_byte((uint8_t *)(DATA_START + offset + i));
		}
		return true;
	}
	uart_print("Reading to EEPROM, nop, erreur nb Magic\n");
	return false;
}

int main(void)
{
	uart_init();

	uint8_t data_to_write[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	uint8_t data_to_write2[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 1};
	uint8_t data_to_read[10] = {0};

	safe_eeprom_write(data_to_write, 0, sizeof(data_to_write));
	safe_eeprom_write(data_to_write, 0, sizeof(data_to_write2));

	// safe_eeprom_read(data_to_read, 0, sizeof(data_to_read));

	return 0;
}

/*---------------------------------------------------*/