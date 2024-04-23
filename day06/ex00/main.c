
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void init_setup()
{
	DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD3);
}

int main(void)
{
	init_setup();
	while (1)
	{
		// rouge
		PORTD |= (1 << PD5);
		_delay_ms(1000);
		PORTD &= ~(1 << PD5);
		// vert
		PORTD |= (1 << PD6);
		_delay_ms(1000);
		PORTD &= ~(1 << PD6);
		// bleue
		PORTD |= (1 << PD3);
		_delay_ms(1000);
		PORTD &= ~(1 << PD3);

		// yellow =rouge et vert
	}
}
