#include <avr/io.h>
#include <util/delay.h>

#define DEBOUNCE_TIME 50 // Temps de rebond en millisecondes

void init_setup()
{
	// Configurer les LED en sortie (pour le portB)
	DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB4);
	// Configurer les bouton en entree (port D)
	DDRD &= ~(1 << DDD2) | (1 << DDD4);
	// Activer la resistance de pull-up pour le bouton
	PORTD |= (1 << PD2) | (1 << PD4);
	// permet d'eviter les etats flottants
}

void display_binary(uint8_t value)
{
	PORTB =
		// ((value & 0x01)) |
		// ((value & 0x02)) |
		// ((value & 0x04)) |
		((value & 0x07)) |
		((value & 0x08) << 1); // Shift bit 3 into position for PB4)
}

int main(void)
{
	init_setup();

	uint8_t counter = 0;
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
					if (counter == 16)
						counter = 0;
					display_binary(counter);
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
					if (counter == 0)
						counter = 16;
					counter--;
					display_binary(counter);
					buttonDecPressedLast = 1;
				}
			}
		}
		else
		{
			buttonDecPressedLast = 0;
		}
	}
	return 0;
}
