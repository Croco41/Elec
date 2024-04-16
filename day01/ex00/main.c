#include <avr/io.h>

// On cherche a creer un delai en utilisant une boucle de retard. Moins precis
// que l'utilisation des timers materiels. Va dependre de la frequence du processeur

// F_CPU 16000000UL // Fréquence du CPU en Hz, par exemple 16 MHz

void my_delay_ms(uint32_t milliseconds)
{
	for (uint32_t i = 0; i < milliseconds; i++)
	{

		// on le force a rien faire pendant un cycle d'horloge
		// volatile est utilisé ici pour indiquer au compilateur que l'instruction
		// ne doit pas être optimisée ou supprimée, même si elle semble ne rien faire d'utile
		asm volatile("nop");
		// ;
	}
}

int main(void)
{
	DDRB |= (1 << DDB1);

	while (1)
	{
		PORTB ^= (1 << PB1);

		my_delay_ms(800000);
	}
}