#include "i2c.h"
#include "uart.h"
#include "timer.h"
#include "game.h"
#include <util/delay.h>

uint8_t error = 0;

void ft_flag_slave(void)
{
    switch (TWSR & 0xF8)
    {
    case TW_SR_SLA_ACK: // SLA+W reçu et ACK renvoyé
        uart_printstr("SLA+W received, ACK returned\r\n");
        break;
    case TW_SR_ARB_LOST_SLA_ACK: // Arbitrage perdu, SLA+W reçu et ACK renvoyé
        uart_printstr("Arbitration lost, SLA+W received, ACK returned\r\n");
        break;
    case TW_SR_GCALL_ACK: // Appel général reçu et ACK renvoyé
        uart_printstr("General call received, ACK returned\r\n");
        break;
    case TW_SR_ARB_LOST_GCALL_ACK: // Arbitrage perdu, appel général reçu et ACK renvoyé
        uart_printstr("Arbitration lost, general call received, ACK returned\r\n");
        break;
    case TW_SR_DATA_ACK: // Données reçues sous adresse esclave ou appel général et ACK renvoyé
        uart_printstr("Data received under slave address, ACK returned\r\n");
        break;
    case TW_SR_DATA_NACK: // Données reçues sous adresse esclave ou appel général et NACK renvoyé
        uart_printstr("Data received under slave address, NACK returned\r\n");
        break;
    case TW_SR_GCALL_DATA_ACK: // Données reçues sous appel général et ACK renvoyé
        uart_printstr("Data received under general call, ACK returned\r\n");
        break;
    case TW_SR_GCALL_DATA_NACK: // Données reçues sous appel général et NACK renvoyé
        uart_printstr("Data received under general call, NACK returned\r\n");
        break;
    case TW_SR_STOP: // STOP ou répétition START reçu
        uart_printstr("STOP or repeated START received\r\n");
        break;
    case TW_ST_SLA_ACK: // SLA+R reçu et ACK renvoyé
        uart_printstr("SLA+R received, ACK returned\r\n");
        break;
    case TW_ST_ARB_LOST_SLA_ACK: // Arbitrage perdu, SLA+R reçu et ACK renvoyé
        uart_printstr("Arbitration lost, SLA+R received, ACK returned\r\n");
        break;
    case TW_ST_DATA_ACK: // Données transmises et ACK reçu
        uart_printstr("Data transmitted, ACK received\r\n");
        break;
    case TW_ST_DATA_NACK: // Données transmises et NACK reçu
        uart_printstr("Data transmitted, NACK received\r\n");
        break;
    case TW_ST_LAST_DATA: // Dernières données transmises et ACK reçu
        uart_printstr("Last data transmitted, ACK received\r\n");
        break;
    case TW_NO_INFO: // Pas d'information disponible
        uart_printstr("No relevant state information available\r\n");
        break;
    case TW_BUS_ERROR: // Erreur de bus, condition illégale
        uart_printstr("Bus error due to an illegal START or STOP condition\r\n");
        break;
    default:
        uart_printstr("Unknown status\r\n");
        break;
    }
}

void slave_button_pressed_counting(void)
{
    if ((PIND & (1 << PD2)) == 0)
    {
        error = 1;
    }
}

void slave_button_pressed(void)
{
    uint8_t button_pressed = 0;

    if ((PIND & (1 << PD2)) == 0 && button_pressed == 0)
    {
        button_pressed = 1;
        // i2c_write(SLAVE_PRESS_SW1);
    }
}

void ft_slave(void)
{
    uint8_t ack = 1;
    uart_printstr("Slave\r\n");
    _delay_ms(1000);
    while (1)
    {
        TWDR = 0;
        TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA);
        if ((TWCR & (1 << TWINT)))
            continue;
        ft_flag_slave();
        if (TWDR == START_GAME)
        {

            led_count(&slave_button_pressed_counting);
            if (error == 1)
            {
                i2c_write(SLAVE_PRESS_SW1);
            }
            while (1)
            {
                slave_button_pressed();
                if ((TWCR & (1 << TWINT)))
                    continue;
                if (TWDR == YOU_WIN)
                {
                    PORTB |= (1 << PB4) | (1 << PB2) | (1 << PB1) | (1 << PB0);
                    break;
                }
                if (TWDR == END_GAME)
                    break;
            }
        }
    }
}