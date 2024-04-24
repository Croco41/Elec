#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*Configuration de l'ADC

	Référence de tension AVCC : Configurez l'ADC pour utiliser AVCC comme référence de tension.
	Résolution de 8 bits : Configurez l'ADC pour une résolution de 8 bits.
	Choix du canal : Sélectionnez le canal correspondant à la broche connectée au potentiomètre.
	Fréquence de l'ADC : Réglez une fréquence appropriée pour l'ADC.
	La fréquence recommandée pour l'ADC est entre 50 kHz et 200 kHz.*/

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

// p257 temperature /voltage

// Fonction pour initialiser le ADC
void init_ADC()
{

	/*   ADLAR (ADC Left Adjust Result) est un bit dans le registre ADMUX. Lorsque ce bit est défini (ADLAR = 1), le résultat de la conversion ADC est ajusté à gauche dans les registres ADCH et ADCL.
	Si ADLAR est réglé à 0, le résultat est ajusté à droite.*/
	// Configurer PC0 comme entrée analogique
	ADMUX |= (1 << REFS0);								  // Référence de tension sur AVCC
	ADMUX |= (1 << ADLAR);								  // Lecture sur ADCH
	ADCSRA |= (1 << ADEN);								  // Activer le convertisseur analogique-numérique
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prédiviseur de fréquence 128 (64:ADPS2,ADPS1)
}

// Fonction pour lire la valeur du potentiomètre
uint8_t read_ADC()
{
	// Lancer la conversion
	ADCSRA |= (1 << ADSC);
	// Attendre la fin de la conversion
	while (ADCSRA & (1 << ADSC))
		;
	// Lire la valeur du convertisseur analogique-numérique
	return ADCH; // result sur 8bits
}

int main(void)
{
	uart_init();
	init_ADC();

	// Lire la valeur du potentiomètre

	while (1)
	{
		uint8_t pot_value = read_ADC();
		print_hexa(pot_value);
		uart_print("\r\n");
		_delay_ms(20);
	}
}