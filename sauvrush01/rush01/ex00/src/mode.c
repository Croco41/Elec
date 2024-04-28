#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "i2c.h"
#include "expander.h"
#include "capteur.h"
#include "mode.h"
#include "mode42.h"
#include "timer.h"

volatile double temp = 0;
volatile double humidity = 0;
volatile uint16_t aht20_run = 0;
volatile uint16_t run_color = 0;
volatile uint16_t color = 0;

void plot_nbr_4(uint16_t n)
{
	cli();
	turn_off_nbrs();
	turn_on_nbr(n - ((n / 10) * 10), 1);
	turn_off_nbrs();
	turn_on_nbr(n / 10 - ((n / 100) * 10), 2);
	turn_off_nbrs();
	turn_on_nbr(n / 100 - ((n / 1000) * 10), 3);
	turn_off_nbrs();
	turn_on_nbr(n / 1000 - ((n / 10000) * 10), 4);
	sei();
}

ISR(TIMER0_COMPA_vect)
{
	uint8_t data[5] = {0};

	aht20_run++;
	run_color++;
	if (aht20_run == 1)
	{
		// uart_print("AHT20\r\n");
		aht20_config();
	}
	if (aht20_run == 81)
	{
		aht20_run = 0;
		aht20_read(data);
		aht20_convert(data, (double *)&temp, (double *)&humidity);
	}
	if (run_color == 500)
	{
		run_color = 0;
		color++;
		if (color == 3)
			color = 0;
	}
}

// Fonction pour initialiser le ADC
void init_ADC(uint8_t volt)
{
	// Configurer PC0 comme entrée analogique
	if (volt)
	{
		ADMUX |= (1 << REFS0) | (1 << REFS1); // Référence de tension sur AVCC
	}
	else
		ADMUX |= (1 << REFS0);
	// ADMUX &= ~(1 << ADLAR);                                  // Lecture sur ADCH
	ADCSRA |= (1 << ADEN);								  // Activer le convertisseur analogique-numérique
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prédiviseur de fréquence 128 (64:ADPS2,ADPS1)
}

// Fonction pour lire la valeur du potentiomètre
uint16_t read_ADC(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	// Lancer la conversion
	ADCSRA |= (1 << ADSC);
	// Attendre la fin de la conversion
	while (ADCSRA & (1 << ADSC))
		;
	// Lire la valeur du convertisseur analogique-numérique
	// return ADCH; // result sur 8bits
	return ADC;
}

uint16_t mode0()
{
	uint16_t time = 0;
	uint16_t count = 0;

	init_ADC(0);
	count = read_ADC(0);
	plot_nbr_4(count);

	return count;
}

uint16_t mode1()
{
	uint16_t time = 0;
	uint16_t count = 0;

	init_ADC(0);
	count = read_ADC(1);
	plot_nbr_4(count);

	return count;
}

uint16_t mode2()
{
	uint16_t time = 0;
	uint16_t count = 0;

	init_ADC(0);
	count = read_ADC(2);
	plot_nbr_4(count);

	return count;
}

void mode3(void)
{
	uint16_t count = 0;

	init_ADC(1);
	count = read_ADC(8);
	count = count * 25 / 314;
	plot_nbr_4(count);
}

uint16_t mode4()
{
	cli();
	// turn_on_3_leds(0xE100FF00, 0xE100FF00, 0xE100FF00);
	// rouge
	if (color == 0)
	{
		PORTD |= (1 << PD5);
		turn_on_3_leds(0xE1FF0000, 0xE1FF0000, 0xE1FF0000); // rouge
	}
	else if (color == 1)
	{
		PORTD |= (1 << PD6); // vert
		turn_on_3_leds(0xE100FF00, 0xE100FF00, 0xE100FF00);
	}
	else if (color == 2)
	{
		PORTD |= (1 << PD3);
		turn_on_3_leds(0xE10000FF, 0xE10000FF, 0xE10000FF); // bleu
	}

	turn_off_nbrs();
	turn_on_nbr(10, 4);
	turn_off_nbrs();
	turn_on_nbr(4, 3);
	turn_off_nbrs();
	turn_on_nbr(2, 2);
	turn_off_nbrs();
	turn_on_nbr(10, 1);
	turn_off_nbrs();
	turn_on_3_leds(0xE1000000, 0xE1000000, 0xE1000000);
	PORTD &= ~(1 << PD5);
	PORTD &= ~(1 << PD6);
	PORTD &= ~(1 << PD3); // noir
	sei();

	return 0;
}

void mode6(void)
{
	plot_nbr_4(temp);
}

void mode7(void)
{
	double tmp = 0;

	// celcius to fahrenheit formul : (°C × 9/5) + 32 = °F
	tmp = (temp * 9 / 5) + 32;
	plot_nbr_4(tmp);
}

void mode8(void)
{
	plot_nbr_4(humidity);
}