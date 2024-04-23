#include "uart.h"
#include "i2c.h"
#include <util/delay.h>

void init_slave()
{
	// page 235
	TWAR = 0x37;
	TWCR = (1 << TWEA) | (1 << TWEN);
}

int main(void)
{
	DDRD &= ~(1 << PD2); // PD2 (INT0) comme entrée
	PORTD |= (1 << PD2); // Activer la résistance de tirage interne
	int slave = 0;
	uart_init();
	i2c_init();
	init_slave();
	TWDR = 0;
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
	{
		uart_printstr("master");
		i2c_start();
		i2c_write(0x00 & (1 << TW_WRITE));
		i2c_stop();
	}
	else
	{
		uart_printstr("slave");
	}
}