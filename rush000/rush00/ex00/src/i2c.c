#include "i2c.h"
#include "uart.h"
#include <stdio.h>

// Pour set la fréquence de l'I2C le SCL avec un prescaler de 1
void i2c_init(void)
{
    TWSR = 0;                          // Set prescaler to 1 page 240
    TWBR = ((F_CPU / F_SCL) - 16) / 2; // (=72) // Set bit rate register page 239
    // TWCR = (1 << TWEN); //i2c material activation
}

// Pour envoyer un start condition et attendre que le bus soit libre
// doc page 225
// uint8_t i2c_start(void)
// {
//     uart_printstr("Start\r\n");
//     TWCR = 0;
//     TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envoie du signal de démarrage
//     while (!(TWCR & (1 << TWINT)))
//         ; // Attente de l'achèvement de l'opération
//     if ((TWSR & 0xF8) != TW_START && (TWSR & 0xF8) != TW_REP_START)
//         return 1;
//     return 0;
// }
uint8_t i2c_start(void)
{
    uart_printstr("Start\r\n");
    TWCR = 0;                                         // Clear TWI control register
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition

    // Debug: Print TWSR before waiting for TWINT to set
    uart_printstr("TWSR before wait: 0x");
    print_hex_value(TWSR & 0xF8); // Masquer les 3 bits de prescaler pour ne montrer que le statut
    uart_printstr("\r\n");

    while (!(TWCR & (1 << TWINT)))
    {
        // Optional: You could add a timeout here to escape the loop if stuck
    }

    // Debug: Print TWSR after TWINT is set
    uart_printstr("TWSR after wait: 0x");
    print_hex_value(TWSR & 0xF8); // Again, mask prescaler bits
    uart_printstr("\r\n");

    if ((TWSR & 0xF8) != TW_START && (TWSR & 0xF8) != TW_REP_START)
    {
        uart_printstr("Failed to start. TWSR: 0x");
        print_hex_value(TWSR & 0xF8);
        uart_printstr("\r\n");
        return 1;
    }
    return 0;
}

// Pour envoyer un stop condition et libérer le bus
// doc page 225
void i2c_stop(void)
{
    uart_printstr("Stop\r\n");
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

// Pour envoyer des données sur le bus I2C et attendre la fin de la transmission
void i2c_write(uint8_t data)
{
    uart_printstr("Write : 0x");
    print_hex_value(data);
    uart_printstr("\r\n");
    TWDR = data;                       // Charger la donnée dans le registre de données TWDR
    TWCR = (1 << TWINT) | (1 << TWEN); // Déclencher la transmission
    while (!(TWCR & (1 << TWINT)))
        ; // Attente de l'achèvement de l'opération
}

// void i2c_write(uint8_t data)
// {
//     uart_printstr("je suis dans le write de debug\r\n");
//     TWDR = data;                       // Charger les données à envoyer
//     TWCR = (1 << TWEN) | (1 << TWINT); // Démarrer la transmission

//     uint32_t timeout = 10000; // Définir un timeout approprié
//     while (!(TWCR & (1 << TWINT)) && --timeout)
//         ; // Attendre la fin de la transmission avec timeout

//     if (timeout == 0)
//     {
//         uart_printstr("Timeout occurred, TWSR: ");
//         print_hex_value(TWSR);
//         uart_printstr("\r\n");
//         // Ajouter ici la logique de gestion du timeout, comme réinitialiser le bus I2C
//         return;
//     }

//     if ((TWSR & 0xF8) == TW_MT_SLA_ACK)
//     {
//         uart_printstr("Address write ACK received.\r\n");
//     }
//     else if ((TWSR & 0xF8) == TW_MT_DATA_ACK)
//     {
//         uart_printstr("Data write ACK received.\r\n");
//     }
//     else
//     {
//         uart_printstr("Failed to receive ACK, TWSR: ");
//         print_hex_value(TWSR);
//         uart_printstr("\r\n");
//     }
//}

// Pour lire les données du bus I2C et envoyer un accusé de réception
void i2c_read(void)
{
    uart_printstr("Read\r\n");
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Déclencher la lecture
    while (!(TWCR & (1 << TWINT)))
        ;
    uart_printstr("Read : 0x");
    print_hex_value(TWDR);
    uart_printstr("\r\n");
}

// La fonction i2c_read_ack() suivante
//  lit une donnée du bus I2C et envoie un ACK automatiquement :
uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Déclencher la lecture avec ACK
    while (!(TWCR & (1 << TWINT)))
        ;                // Attendre que la lecture soit complète
    uint8_t data = TWDR; // Lire la donnée reçue
    return data;         // Retourner la donnée lue
}

/* i2c_read_nack() est utilisée pour lire la dernière donnée d'une séquence de données.
Elle envoie un NACK pour indiquer à l'émetteur de stopper l'envoi des données :*/
uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN); // Déclencher la lecture sans ACK (NACK)
    while (!(TWCR & (1 << TWINT)))
        ;                // Attendre que la lecture soit complète
    uint8_t data = TWDR; // Lire la donnée reçue
    return data;         // Retourner la donnée lue
}

void setup_addr(uint8_t addr)
{
    TWAR = (addr << 1) | 1; // Set slave address
    // Mettre ce bit à 1 permet à l'esclave de répondre à l'adresse générale de broadcast 0x00.
    TWCR = (1 << TWEA) | (1 << TWEN); // Enable TWI and ack
}

void setup_addr2(uint8_t addr)
{
    TWAR = (addr << 1) | 0; // Set slave address
    // Mettre ce bit à 1 permet à l'esclave de répondre à l'adresse générale de broadcast 0x00.
    TWCR = (1 << TWEA) | (1 << TWEN); // Enable TWI and ack
}
void setup_addr_master(uint8_t addr)
{
    TWAR = (addr << 1) | 0; // Set master address
    // Mettre ce bit à 1 permet à l'esclave de répondre à l'adresse générale de broadcast 0x00.
    TWCR = (1 << TWINT) | (1 << TWEN); // Enable TWI and ack
}