#include <avr/io.h>

// On cherche a creer un delai en utilisant une boucle de retard. Moins precis
// que l'utilisation des timers materiels. Va dependre de la frequence du processeur

// F_CPU 16000000UL // Fréquence du CPU en Hz, par exemple 16 MHz

void my_delay_ms(int milliseconds)
{
	for (int i = 0; i < milliseconds; i++)
	{
		for (int j = 0; j < 2000; j++)
		{
			asm volatile("nop");
		}
	}
}

int main(void)
{
	DDRB |= (1 << DDB1);

	while (1)
	{
		PORTB ^= (1 << PB1);

		my_delay_ms(1000);
	}
}