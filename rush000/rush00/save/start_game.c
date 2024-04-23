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

void start_game_master(void)
{
	if (!(PIND & (1 << PD2)))
	{
		PORTD |= (1 << PD5);
		// Anti-rebond
		_delay_ms(20);
		i2c_send_ready_signal(); // pas codee

		// Attendre la confirmation de l'esclave.
		if (i2c_wait_for_slave_ready())
		{
			// Commencer le compte à rebours.
			init_setup_led();
			random_delay();
			start_countdown();
		}
	}
}
