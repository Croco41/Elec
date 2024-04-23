#include "i2c.h"
#include "uart.h"
#include "timer.h"
#include "game.h"
#include <util/delay.h>

volatile uint8_t button_pressed = 0;
volatile uint8_t error = 0;
uint8_t slave_win = 2;

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
    if ((PIND & (1 << PD2)) == 0)
    {
        if (!button_pressed)
        {
            uart_printstr("Button pressed, preparing to notify master.\r\n");
            button_pressed = 1;
        }
    }
    else
    {
        button_pressed = 0; // Bouton relâché
    }
}

void respond_to_master_request()
{
    if (button_pressed)
    {
        uart_printstr("Button pressed, notifying master.\r\n");
        TWDR = SLAVE_PRESS_SW1; // Code pour bouton pressé
        button_pressed = 0;     // Réinitialisation de l'état du bouton
        uart_printstr("Response loaded: 0x");
        print_hex_value(TWDR); // Assurez-vous que cette fonction imprime un uint8_t en hexadécimal
        uart_printstr("\r\n");
    }
    else
    {
        // uart_printstr("Button PAS pressed, notifying master.\r\n");
        TWDR = SLAVE_NOT_PRESS_SW1; // Code pour bouton non pressé
    }
    // Activation de TWI, acquittement et remise à zéro de TWINT pour commencer la transmission
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT);
    uart_printstr("Response loaded: 0x");
    print_hex_value(TWDR); // Assurez-vous que cette fonction imprime un uint8_t en hexadécimal
    uart_printstr("\r\n");
    // }
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
            led_count();
            // mettre la communication avec Master qui attend SLAVER_PRESS_SW1
        }
        setup_addr2(SLAVE_ADDR);
        while (1)
        {
            TWDR = 0;
            TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA);
            // uart_printstr("Boucle slave communication.\r\n");
            //  Vérifie si l'état I2C indique que le maître demande des données
            slave_button_pressed();
            if ((TWCR & (1 << TWINT)))
                continue;
            // while (!(TWCR & (1 << TWINT)))
            //     ;
            if (TWDR == DO_YOU_HAVE_PRESS_SW1)
            {
                // Répondre à la requête du maître seulement s'il a envoyé un signal de start
                uart_printstr("tu es dans doyouHavePressSw1");
                respond_to_master_request();
            }
            else if (TWDR == YOU_LOSE)
            {
                uart_printstr("Tu as perdu, fin du jeu!!!");
                led_lose();
                break;
            }
            else if (TWDR == YOU_WIN)
            {
                uart_printstr("Tu as gagne!!!");
                led_win();
                uart_printstr("FIN du Jeu");
                break;
            }
        }
        break;
    }
}