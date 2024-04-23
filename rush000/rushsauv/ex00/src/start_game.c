#include "i2c.h"
#include "uart.h"
#include <util/delay.h>

/*-------------------Bouton + LED VERTE------------------------*/

void setup(void)
{
	// LED GREEN
	//  Configurer PD5 en sortie pour la LED
	DDRD |= (1 << PD5); // PD5 est maintenant configurée en sortie

	// BOUTON
	//  Configurer le bouton en entrée
	DDRD &= ~(1 << PD2);
	// Activer la résistance de pull-up
	PORTD |= (1 << PD2);
	// permet d'eviter les etats flottants
}

void start_game(void)
{
	if (!(PIND & (1 << PD2)))
	{
		PORTD |= (1 << PD5);
		// Anti-rebond
		_delay_ms(20);
	}
}
