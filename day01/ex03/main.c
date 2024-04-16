#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 50 // Temps de rebond en millisecondes

// F_CPU 16000000UL // Fréquence du CPU en Hz, par exemple 16 MHz

/*Le rapport cyclique de 10 % signifie que la LED sera allumée pendant
10 % de la période et éteinte pendant 90 % de la période. Si la fréquence est de 1 Hz,
la période est de 1 seconde (1000 ms), donc la LED sera allumée pendant 100 ms et éteinte pendant 900 ms.
*/

// Mode PWM (Pulse Width Modulation) : Génération de signaux PWM pour contrôler des moteurs, des LEDs, etc.

/*Configurer le Timer1:

	Choisissez le mode Fast PWM où ICR1 définit le TOP pour le Timer.
	Réglez le prédiviseur pour obtenir la fréquence désirée.
	Configurez la sortie OC1A pour qu'elle soit activée à la comparaison et désactivée au début du cycle.*/

/*Définir ICR1 et OCR1A:

	ICR1 définit la période totale du PWM.
	OCR1A définit le point où la sortie passe de haut à bas, contrôlant ainsi le rapport cyclique.*/

void setup()
{
	DDRB |= (1 << DDB1);

	// Configurer les bouton en entree (port D)
	DDRD &= ~(1 << DDD2) | (1 << DDD4);
	// Activer la resistance de pull-up pour le bouton
	PORTD |= (1 << PD2) | (1 << PD4);
	// permet d'eviter les etats flottants

	TCCR1A = 0;
	TCCR1B = 0;
	// Configure Timer1 for Fast PWM mode 14, ICR1 is TOP
	// TCCR1A=0b10100011;
	TCCR1A = (1 << COM1A1) | (1 << WGM11);				// Clear OC1A on Compare Match, set at BOTTOM
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12); // precaler de 256
	// Set ICR1 for a period of 1 Hz (f = 16 MHz / (256 * (1 + ICR1)), ICR1 = 24999 for ~1Hz)
	// fpwm = (fCPU/(Prescaler * (1 + ICR1)))
	ICR1 = 62500;

	// Set OCR1A for a duty cycle of 10%
	OCR1A = 6250; // 10% of 62500
}

int main(void)
{
	setup();

	uint8_t counter = 1;
	uint8_t buttonIncPressedLast = 0;
	uint8_t buttonDecPressedLast = 0;

	while (1)
	{
		// Gestion du bouton d'incrémentation
		if (!(PIND & (1 << PD2)))
		{
			if (!buttonIncPressedLast)
			{
				_delay_ms(DEBOUNCE_TIME);
				if (!(PIND & (1 << PD2)))
				{
					counter++;
					if (counter == 11)
						counter = 1;
					OCR1A = 6250 * counter; // 10% of 62500
					// display_binary(counter);
					buttonIncPressedLast = 1;
				}
			}
		}
		else
		{
			buttonIncPressedLast = 0;
		}

		// Gestion du bouton de décrémentation
		if (!(PIND & (1 << PD4)))
		{
			if (!buttonDecPressedLast)
			{
				_delay_ms(DEBOUNCE_TIME);
				if (!(PIND & (1 << PD4)))
				{
					counter--;
					if (counter == 0)
						counter = 10;
					OCR1A = 6250 * counter; // 10% of 62500
					// display_binary(counter);
					buttonDecPressedLast = 1;
				}
			}
		}
		else
		{
			buttonDecPressedLast = 0;
		}
		// OCR1A = 6250 * counter; // 10% of 62500
	}
	return 0;
}
