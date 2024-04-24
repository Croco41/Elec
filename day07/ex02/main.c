#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*Configuration de l'ADC

	Référence de tension AVCC : Configurez l'ADC pour utiliser AVCC comme référence de tension.
	Résolution de 8 bits : Configurez l'ADC pour une résolution de 8 bits.
	Choix du canal : Sélectionnez le canal correspondant à la broche connectée au potentiomètre.
	Fréquence de l'ADC : Réglez une fréquence appropriée pour l'ADC.
	La fréquence recommandée pour l'ADC est entre 50 kHz et 200 kHz.*/

//(un potentiomètre, une résistance dépendante de la lumière (LDR), et une thermistance NTC)

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

// p257 temperature /voltage

/*Étapes pour lire plusieurs entrées analogiques :

	Initialisation de l'ADC : Configurez l'ADC pour utiliser AVCC comme tension de référence
	et réglez la résolution à 8 bits, si nécessaire.
	Sélection des canaux d'entrée : Pour chaque lecture, sélectionnez le canal
	d'entrée correspondant (RV1, LDR, NTC) via le registre ADMUX.
	Lecture et conversion : Démarrez une conversion, attendez qu'elle soit terminée,
	et lisez le résultat.
	Affichage des résultats : Convertissez les valeurs lues en format hexadécimal et
	envoyez-les à l'UART pour affichage.*/

// Fonction pour initialiser le ADC
void init_ADC()
{

	/*   ADLAR (ADC Left Adjust Result) est un bit dans le registre ADMUX. Lorsque ce bit est défini (ADLAR = 1), le résultat de la conversion ADC est ajusté à gauche dans les registres ADCH et ADCL.
	Si ADLAR est réglé à 0, le résultat est ajusté à droite.*/
	// Configurer PC0 comme entrée analogique
	ADMUX |= (1 << REFS0);								  // Référence de tension sur AVCC
	ADMUX &= ~(1 << ADLAR);								  // bit ADLAR a 0 =>Lecture sur ADCH etADCL
	ADCSRA |= (1 << ADEN);								  // Activer le convertisseur analogique-numérique
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prédiviseur de fréquence 128 (64:ADPS2,ADPS1)
}

uint16_t read_adc(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel with safety mask
	ADCSRA |= (1 << ADSC);					   // Start conversion
	while (ADCSRA & (1 << ADSC))
		;					  // Wait for conversion to finish
	uint8_t low = ADCL;		  // Read low byte first
	uint16_t high = ADCH;	  // Read high byte
	return (high << 8) | low; // Combine high and low byte
}

// uint16_t read_adc2(uint8_t channel)
// {
// 	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel with safety mask
// 	ADCSRA |= (1 << ADSC);					   // Start conversion
// 	while (ADCSRA & (1 << ADSC))
// 		;				  // Wait for conversion to finish
// 	uint8_t low = ADCL;	  // Read low byte first
// 	uint16_t high = ADCH; // Read high byte
// 	return ADC;			  // Combine high and low byte
// }

int main()
{
	uart_init();
	init_ADC();

	while (1)
	{
		uint16_t pot_value = read_adc(0); // Channel for RV1 (tous les bitsa 0)
		uint16_t ldr_value = read_adc(1); // Channel for LDR ( le dernier a 1)
		uint16_t ntc_value = read_adc(2); // Channel for NTC (l'avantdernier a 1)

		// uint8_t pot_value2 = read_adc2(0); // Channel for RV1 (tous les bitsa 0)
		// uint8_t ldr_value2 = read_adc2(1); // Channel for LDR ( le dernier a 1)
		// uint8_t ntc_value2 = read_adc2(2); // Channel for NTC (l'avantdernier a 1)

		uart_print(avr_itoa(pot_value));
		uart_print(", ");
		uart_print(avr_itoa(ldr_value));
		uart_print(", ");
		uart_print(avr_itoa(ntc_value));
		uart_print("\r\n");

		// print_big_hexa(pot_value2);
		// uart_print(", ");
		// print_big_hexa(ldr_value2);
		// uart_print(", ");
		// print_big_hexa(ntc_value2);
		// uart_print("\r\n");
		// uart_print("\r\n");

		_delay_ms(20);
	}
}