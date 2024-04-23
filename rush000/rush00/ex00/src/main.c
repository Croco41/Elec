#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include <util/delay.h>

int main(void)
{
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4); // PB0, PB1, PB2, PB4 comme sortie
	DDRD &= ~(1 << PD2);									   // PD2 (INT0) comme entrée
	PORTD |= (1 << PD2);									   // Activer la résistance de tirage interne
	int slave = 0;
	timer_init();
	uart_init();
	i2c_init();
	setup_addr(SLAVE_ADDR);

	while (1)
	{
		if (!(PIND & (1 << PD2)))
		{
			while (!(PIND & (1 << PD2)))
			{
				_delay_ms(1);
			}
			break;
		}
		if ((TWCR & (1 << TWINT)))
		{
			slave = 1;
		}
	}
	if (!slave)
		ft_master();
	else
		ft_slave();
	return 0;
}