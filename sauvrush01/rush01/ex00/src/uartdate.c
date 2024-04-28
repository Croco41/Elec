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

int validate_date_time(const char *str)
{
	int day, hour, minute, second;
	int year, month;
	if (sscanf(str, "%2d/%2d/%4d %2d:%2d:%2d", &day, &month, &year, &hour, &minute, &second) == 6)
	{
		// Vérification de la validité de l'année
		if (year < 2000 || year > 2200)
		{
			uart_print("invalide year\r\n");
			return 0;
		}
		// Vérification de la validité du mois
		if (month < 1 || month > 12)
		{
			uart_print("invalide month\r\n");
			return 0;
		}
		// Vérification de la validité du jour
		if (day < 1 || day > 31)
		{
			uart_print("invalide day\r\n");
			return 0;
		}
		// Vérifications spécifiques pour chaque mois
		if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
		{
			uart_print("invalide jour dans mois\r\n");
			return 0; // Avril, Juin, Septembre et Novembre n'ont que 30 jours
		}
		if (month == 2)
		{ // Février
			// Vérification d'une année bissextile
			if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
			{
				if (day > 29)
				{
					uart_print("toto\r\n");
					return 0; // Février d'une année bissextile a au maximum 29 jours
				}
			}
			else
			{
				if (day > 28)
				{
					uart_print("titi\r\n");
					return 0; // Février d'une année non bissextile a au maximum 28 jours
				}
			}
		}
		// Vérification des heures, minutes et secondes
		if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59)
		{
			uart_print("verif des linites h m s\r\n");
			return 0;
		}
		update_time(to_bcd((uint8_t)second), to_bcd((uint8_t)minute), to_bcd((uint8_t)hour), to_bcd((uint8_t)day), to_bcd((uint8_t)month), to_bcd((uint8_t)(year - (year / 100) * 100)));
		return 1; // Tout est valide
	}
	return 0; // Format incorrect
}

void uarttimesw3()
{
	char inputBuffer[20] = {0}; // Taille ajustée pour "DD/MM/YYYY HH:MM:SS" + '\0'
	int index = 0;
	char c;
	uart_print("Enter date and time (DD/MM/YYYY HH:MM:SS): ");

	while ((c = uart_rx()) != ENTER && index < 19)
	{
		if (c == BACKSPACE && index > 0)
		{
			uart_tx('\b');
			uart_tx(' ');
			uart_tx('\b');
			index--;
		}
		else if (c >= '0' && c <= '9' || c == '/' || c == ' ' || c == ':')
		{
			inputBuffer[index++] = c;
			uart_tx(c);
		}
	}
	inputBuffer[index] = '\0'; // Assurez-vous que la chaîne est terminée correctement

	uart_tx('\r');
	uart_tx('\n');

	if (validate_date_time(inputBuffer))
	{
		uart_print("Valid date format.\n");
		// Vous pouvez ici ajouter le code pour configurer la date et l'heure dans votre système
	}
	else
	{
		uart_print("Invalid date format!\n");
	}
}
