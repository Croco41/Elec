#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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

// Fonction pour initialiser le ADC
void init_ADC()
{

	/*   ADLAR (ADC Left Adjust Result) est un bit dans le registre ADMUX. Lorsque ce bit est défini (ADLAR = 1), le résultat de la conversion ADC est ajusté à gauche dans les registres ADCH et ADCL.
	Si ADLAR est réglé à 0, le résultat est ajusté à droite.*/
	// Configurer PC0 comme entrée analogique
	ADMUX |= (1 << REFS0) | (1 << REFS1); // Référence de tension sur AVCC
	// ADMUX &= ~(1 << ADLAR);								  // bit ADLAR a 0 =>Lecture sur ADCH etADCL
	ADCSRA |= (1 << ADEN);								  // Activer le convertisseur analogique-numérique
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prédiviseur de fréquence 128 (64:ADPS2,ADPS1)
}

uint16_t read_adc(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel with safety mask
	ADCSRA |= (1 << ADSC);					   // Start conversion
	while (ADCSRA & (1 << ADSC))
		; // Wait for conversion to finish

	return ADC; // Combine high and low byte
}

// int convert_to_celsius(uint16_t adc_value)
// {
// 	// According to the datasheet, at 1.1V reference, 10 mV/°C scale factor
// 	return (adc_value * 1.1 / 1024.0 - 0.5) * 100;
// }

int main()
{
	uart_init();
	init_ADC();

	while (1)
	{
		uint16_t temp_value = read_adc(8); // ADC08

		// uint16_t temp_c = convert_to_celsius(temp_value);
		uint16_t temp_c = temp_value * 25 / 314;
		uart_print(avr_itoa(temp_c));
		uart_print("\r\n");
		_delay_ms(20);
	}
}