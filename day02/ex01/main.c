#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 50 // Temps de rebond en millisecondes

void setup()
{
	DDRB |= (1 << PB1);

	TCCR1A |= (1 << COM1A1) | (1 << WGM11);				 // Clear OC1A on Compare Match, set at BOTTOM
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS12); // precaler de 256

	// fpwm = (fCPU/(Prescaler * (1 + ICR1)))
	ICR1 = 100;

	OCR1A = 0; //

	// SETUP TIMER0:
	TCCR0A |= (1 << WGM01); // modeCTC
	// TCCR0A = 0x00;						 // Normal mode
	TCCR0B |= (1 << CS02) | (1 << CS00); // Prescaler 1024 // obliger on est sur 8bits

	TIMSK0 |= (1 << OCIE0A); // interrupt quand TCNT0 compare OCR0A;
	// FCPU/1024/200
	OCR0A = 78;
}

//	0x001C	TIMER0 COMPA	Timer/Counter0 Compare Match A	(TIMER0_COMPA_vect)	OC0Aaddr
ISR(TIMER0_COMPA_vect)
{
	static uint8_t up = 1;

	if (up)
	{
		OCR1A++;
		// Increase the duty cycle
		if (OCR1A >= ICR1)
		{
			up = 0; // Start decreasing after reaching 100%
		}
	}
	else
	{
		OCR1A--; // Decrease the duty cycle
		if (OCR1A == 0)
		{
			up = 1; // Start increasing after reaching 0%
		}
	}
}

int main(void)
{
	setup();
	sei();
	while (1)
	{
	}
	return 0;
}
