
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define RED_LED_PIN PD5
#define GREEN_LED_PIN PD6
#define BLUE_LED_PIN PD3

void init_rgb()
{
	DDRD |= (1 << RED_LED_PIN) | (1 << GREEN_LED_PIN) | (1 << BLUE_LED_PIN);

	// timers pour le mode Fast PWM
	// Par exemple, pour le Timer0 (associé à PD5 et PD6 pour OC0A et OC0B respectivement)
	/*Timer0 en mode Fast PWM, ce qui signifie que le compteur du timer
	s'incrémente jusqu'à atteindre la valeur maximale (0xFF ou 255 pour un timer 8 bits), puis recommence à 0.*/
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // Mode Fast PWM

	/*Configure les sorties OC0A (PD5) et OC0B (PD6) pour qu'elles passent à 0
	lorsqu'une correspondance est trouvée entre TCNT0 (la valeur du timer) et
	OCR0A/OCR0B, et qu'elles repassent à 1 lorsque le timer repasse à 0*/
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1); // Clear OC0A/OC0B on Compare Match, set OC0A/OC0B at BOTTOM
	// TCCR0B |= (1 << CS01);					 // Prescaler de 8, peut-être ajusté pour une fréquence différente
	TCCR0B |= (1 << CS00);

	// Timer 2 (pour PD3 - OC2B)
	TCCR2A |= (1 << WGM21) | (1 << WGM20); // Mode Fast PWM
	TCCR2A |= (1 << COM2B1);			   // Clear OC2B on Compare Match, set at BOTTOM (non-inverting mode)
	// TCCR2B |= (1 << CS21);				   // Prescaler de 8, ajuster pour la fréquence désirée
	TCCR2B |= (1 << CS20);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	/*ces registres contrôle la durée pendant laquelle une LED spécifique est allumée
	au cours d'un cycle de PWM, permettant ainsi de moduler l'intensité perçue
	de chaque couleur de la LED RGB*/
	/*Définit la durée pendant laquelle la LED rouge est allumée dans chaque cycle de PWM.
	Une valeur de 0 signifie que la LED est toujours éteinte,
	une valeur de 255 signifie qu'elle est toujours allumée,
	et des valeurs intermédiaires donnent différents niveaux de luminosité*/

	// Assumer que OC0A est pour rouge, OC0B pour vert et un autre Timer/Compare pour bleu
	OCR0A = g; // Définir la largeur de l'impulsion pour rouge
	OCR0B = r; // Définir la largeur de l'impulsion pour vert
	OCR2B = b; // ... Et ainsi pour bleu avec le bon Timer/OCR
}

void wheel(uint8_t pos)
{
	pos = 255 - pos;
	if (pos < 85)
	{
		set_rgb(255 - pos * 3, 0, pos * 3);
	}
	else if (pos < 170)
	{
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	}
	else
	{
		pos = pos - 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

// Initialiser le Timer 1 pour déclencher une interruption à chaque milliseconde
// void init_timer1()
// {
// 	TCCR1B |= (1 << WGM12); // Mode CTC
// 	OCR1A = 2499;			// 16MHz / 64 prescaler / 100Hz - 1 = 2499
// 	// OCR1A = 3000;
// 	TIMSK1 |= (1 << OCIE1A);			 // Activer l'interruption de comparaison A pour Timer1
// 	TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler de 64
// }

// ISR(TIMER1_COMPA_vect)
// {
// 	// Cette interruption sera appelée à chaque fois que TCNT1 atteint OCR1A
// 	static uint8_t pos = 0;
// 	wheel(pos++);
// 	if (pos > 255)
// 		pos = 0;
// }

int main()
{
	init_rgb(); // Initialise le PWM pour RGB
	// init_timer1();
	// sei();
	uint8_t pos = 0;
	while (1)
	{
		wheel(pos++);  // On fait la roue des couleurs
		_delay_ms(20); // pour contrôler la vitesse de transition des couleurs
		// il faut utilser les timers a la place des delay
		if (pos > 255)
			pos = 0; // Réinitialiser si dépassement
	}
	return 0;
}
