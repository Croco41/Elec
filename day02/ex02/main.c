#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DEBOUNCE_TIME 50 // Temps de rebond en millisecondes

static uint8_t value;

void init_setup()
{
	sei();
	// Configurer les LED en sortie (pour le portB)
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);

	PORTD |= (1 << PD2) | (1 << PD4);
	// permet d'eviter les etats flottants

	EICRA |= (1 << ISC01); // quand on appuie (le descendant)
	EIMSK |= (1 << INT0);  // activer INT0

	PCICR |= (1 << PCIE2);	  // quand on appuie (le descendant)
	PCMSK2 |= (1 << PCINT20); // activer le bouton sw2
}

void display_binary(uint8_t value)
{
	PORTB =
		((value & 0x07)) |
		((value & 0x08) << 1); // Shift bit 3 into position for PB4)
}

ISR(INT0_vect)
{
	// code qui s'execute quand on appuie sur sw1 (INTO_vect)
	value++;
	display_binary(value);
	_delay_ms(DEBOUNCE_TIME);
}

ISR(PCINT2_vect)
{
	// code qui s'execute quand on appuie sur sw1 (PCINT2_vect)
	static uint8_t button_dec = 0;

	if (!button_dec)
	{
		value--;
		display_binary(value);
		button_dec = 1;
		_delay_ms(DEBOUNCE_TIME);
	}
	else
	{
		button_dec = 0;
	}
}

int main(void)
{
	init_setup();

	while (1)
	{
	}
	return 0;
}
