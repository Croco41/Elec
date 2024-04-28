#include "uart.h"
#include "i2c.h"
#include "timer.h"
#include "expander.h"
#include "mode.h"
#include "mode42.h"
#include "uartdate.h"
#include "date.h"
#include <util/delay.h>
#include <avr/interrupt.h>

void setup()
{
	DDRD &= ~(1 << DDD2) | (1 << DDD4);
	// Activer la resistance de pull-up pour le bouton
	PORTD |= (1 << PD2) | (1 << PD4);
	// permet d'eviter les etats flottants
}

void turn_on_sw(uint8_t sw1, uint8_t sw2, uint8_t sw3)
{
	i2c_start();
	i2c_write(0x20 << 1 | 0);
	i2c_write(0x02);
	i2c_write(0b11111111);
	i2c_stop();

	uint8_t code = 0b11111111;
	if (sw1)
		code &= ~(0b00001000);
	if (sw2)
		code &= ~(0b00000100);
	if (sw3)
		code &= ~(0b00000010);

	i2c_start();
	i2c_write(0x20 << 1 | 0);
	i2c_write(0x02);
	i2c_write(code);
	i2c_stop();
}

void display_binary(uint8_t value)
{
	PORTB =
		((value & 0x07)) |
		((value & 0x08) << 1); // Shift bit 3 into position for PB4)
}

void switch_mode(uint8_t mode)
{
	switch (mode)
	{
	case 0:
		mode0();
		break;
	case 1:
		mode1();
		break;
	case 2:
		mode2();
		break;
	case 3:
		mode3();
		break;
	case 4:
		mode4();
		break;
	// case 5:
	// 	mode5();
	// 	break;
	case 6:
		mode6();
		break;
	case 7:
		mode7();
		break;
	case 8:
		mode8();
		break;
	case 9:
		mode9();
		break;
	case 10:
		mode10();
		break;
	case 11:
		mode11();
		break;
	default:
		uart_print("Invalid mode\r\n");
	}
}

int main(void)
{
	uart_init();
	i2c_init();
	timer_init();
	setup_pca9555();
	SPI_masterInit();
	init_D5();
	// update_time(to_bcd(0), to_bcd(33), to_bcd(18), to_bcd(28), to_bcd(4), to_bcd(24));

	setup();

	startRush(); // il faudra implementer interupt pour pouvoir appuyer sur SW1, Sw2, SW3

	uint8_t mode = 9;
	uint8_t buttonIncPressedLast = 0;
	uint8_t buttonDecPressedLast = 0;

	uint8_t sw1on = 0;
	uint8_t sw2on = 0;
	uint8_t sw3on = 0;
	uint8_t reg = 0;

	cli();
	while (1)
	{
		sw1on = 0;
		sw2on = 0;
		sw3on = 0;

		if (!(PIND & (1 << PD2)))
		{
			sw1on = 1;
			if (!buttonIncPressedLast)
			{
				_delay_ms(50);
				if (!(PIND & (1 << PD2)))
				{
					mode++;
					if (mode == 12)
						mode = 0;
					else if (mode == 5)
						mode = 6;
					display_binary(mode);
					buttonIncPressedLast = 1;
				}
			}
		}
		else
		{
			buttonIncPressedLast = 0;
		}
		if (!(PIND & (1 << PD4)))
		{
			sw2on = 1;
			if (!buttonDecPressedLast)
			{
				_delay_ms(50);
				if (!(PIND & (1 << PD4)))
				{
					if (mode == 0)
						mode = 12;
					else if (mode == 5)
						mode = 4;
					mode--;
					display_binary(mode);
					buttonDecPressedLast = 1;
				}
			}
		}
		else
		{
			buttonDecPressedLast = 0;
		}

		// Read SW3
		cli();
		i2c_start();
		i2c_write(0x20 << 1);
		i2c_write(0x00);

		i2c_start();
		i2c_write(0x20 << 1 | 1);
		reg = i2c_read(0);
		i2c_stop();

		if (!(reg & 0b00000001))
		{
			sw3on = 1;
		}

		turn_on_sw(sw1on, sw2on, sw3on);
		if (sw3on == 1)
			uarttimesw3();
		sei();
		switch_mode(mode);
		cli();
	}
	return 0;
}
