#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "i2c.h"
#include "expander.h"
#include "capteur.h"
#include "mode.h"
#include "mode42.h"
#include "timer.h"
#include "uartdate.h"
#include "date.h"
#include <stdio.h>

enum time
{
	HOURS,
	MINUTES,
	SECONDS
};

enum date
{
	DAY,
	WEEKDAY,
	MONTH,
	YEAR
};

volatile uint8_t time[3] = {0};
volatile uint8_t date[4] = {0};
volatile uint8_t vl = 0;

// volatile uint8_t yearG = 20;

void read_time()
{
	i2c_start();
	i2c_write((PCF8563_ADDRESS << 1));
	i2c_write(REG_SECONDS);
	i2c_start();
	i2c_write((PCF8563_ADDRESS << 1) | 1);
	time[SECONDS] = i2c_read(1); // Secondes
	time[MINUTES] = i2c_read(1); // Minutes
	time[HOURS] = i2c_read(1);	 // Heures
	date[DAY] = i2c_read(1);	 // Jour
	date[WEEKDAY] = i2c_read(1); // Jour de la semaine
	date[MONTH] = i2c_read(1);	 // Mois
	date[YEAR] = i2c_read(0);	 // Année
	i2c_stop();

	time[SECONDS] = ((time[SECONDS] & 0x70) >> 4) * 10 + (time[SECONDS] & 0x0F);
	time[MINUTES] = ((time[MINUTES] & 0x70) >> 4) * 10 + (time[MINUTES] & 0x0F);
	time[HOURS] = ((time[HOURS] & 0x30) >> 4) * 10 + (time[HOURS] & 0x0F);

	date[DAY] = ((date[DAY] & 0x30) >> 4) * 10 + (date[DAY] & 0x0F);
	if (date[MONTH] & 0x80)
		vl = 1;
	else
		vl = 0;
	date[MONTH] = ((date[MONTH] & 0x10) >> 4) * 10 + (date[MONTH] & 0x0F);
	date[YEAR] = ((date[YEAR] & 0xF0) >> 4) * 10 + (date[YEAR] & 0x0F);
}

void update_time(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t month, uint8_t year)
{
	i2c_start();
	i2c_write(PCF8563_ADDRESS << 1);
	i2c_write(REG_SECONDS);
	i2c_write(seconds);
	i2c_write(minutes);
	i2c_write(hours);
	i2c_write(day);
	i2c_write(0);
	i2c_write(month);
	i2c_write(year);
	i2c_stop();
}

// void loop()
// {
// 	char str[255] = {0};

// 	read_time(time, date);
// 	uart_printint(time[HOURS]);
// 	uart_tx(':');
// 	uart_printint(time[MINUTES]);
// 	uart_tx(':');
// 	uart_printint(time[SECONDS]);
// 	uart_tx(' ');
// 	uart_printint(date[DAY]);
// 	uart_tx('/');
// 	uart_printint(date[MONTH]);
// 	uart_tx('/');
// 	uart_printint(date[YEAR]);
// 	uart_printstr("\r\n");
// 	uart_printstr(str);
// }

// Displays the time in 24-hour format (23:23).

uint8_t to_bcd(uint8_t n)
{
	return (n / 10 << 4 | (n - (n / 10) * 10));
}

void mode9()
{
	cli();
	read_time();
	turn_off_nbrs();
	turn_on_nbr(time[HOURS] / 10, 4);
	turn_off_nbrs();
	turn_on_nbr(time[HOURS] - (time[HOURS] / 10) * 10, 3);
	turn_off_nbrs();
	turn_on_nbr(time[MINUTES] / 10, 2);
	turn_off_nbrs();
	turn_on_nbr(time[MINUTES] - (time[MINUTES] / 10) * 10, 1);
	turn_off_nbrs();
	manual_turn_on(0b01111111, 0b10000000);
	turn_off_nbrs();
	manual_turn_on(0b11011111, 0b10000000);
	turn_off_nbrs();
	sei();
}

void mode10()
{
	cli();
	read_time();
	turn_off_nbrs();
	turn_on_nbr(date[DAY] / 10, 4);
	turn_off_nbrs();
	turn_on_nbr(date[DAY] - (date[DAY] / 10) * 10, 3);
	turn_off_nbrs();
	turn_on_nbr(date[MONTH] / 10, 2);
	turn_off_nbrs();
	turn_on_nbr(date[MONTH] - (date[MONTH] / 10) * 10, 1);
	turn_off_nbrs();
	manual_turn_on(0b01111111, 0b10000000);
	turn_off_nbrs();
	manual_turn_on(0b11011111, 0b10000000);
	turn_off_nbrs();
	sei();
}

void mode11()
{
	static uint8_t year = 20;
	static bool year_passed = false;

	cli();
	read_time();
	if (vl && !year_passed)
	{
		year++;
		year_passed = true;
	}
	else if (!vl && year_passed)
	{
		year_passed = false;
	}
	turn_off_nbrs();
	turn_on_nbr(year / 10, 4);
	turn_off_nbrs();
	turn_on_nbr(year - (year / 10) * 10, 3);
	turn_off_nbrs();
	turn_on_nbr(date[YEAR] / 10, 2);
	turn_off_nbrs();
	turn_on_nbr(date[YEAR] - (date[YEAR] / 10) * 10, 1);
	turn_off_nbrs();
	sei();
}