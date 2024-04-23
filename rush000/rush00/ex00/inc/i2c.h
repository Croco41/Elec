#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
#include <util/twi.h>

#define F_SCL 100000UL // 100kHz I2C clock speed
#define WAIT_BIT (double)((1 / F_SCL) * 1000000)

#define MASTER_ADDR 0x10
#define SLAVE_ADDR 0x11

#define BROADCAST_MSG 0x00
#define START_GAME 0xF1
#define END_GAME 0xF2
#define READY 0xF3
#define SLAVE_PRESS_SW1 0xFF
#define SLAVE_NOT_PRESS_SW1 0xF1
#define YOU_WIN 0x0A
#define YOU_LOSE 0xF3
#define DO_YOU_HAVE_PRESS_SW1 0x0C

#define NULL ((void *)0)

void i2c_init(void);
uint8_t i2c_start(void);
void i2c_stop(void);

void i2c_write(uint8_t data);
void i2c_read(void);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

void setup_addr(uint8_t addr);
void setup_addr2(uint8_t addr);
void setup_addr_master(uint8_t addr);

void ft_master(void);
void ft_slave(void);
void respond_to_master_request(void);

#endif

/* I2C Communication
L'I2C (Inter-Integrated Circuit) est un protocole de communication qui permet à plusieurs dispositifs de communiquer entre eux via un bus de données bidirectionnel. Il est souvent utilisé pour la communication entre les microcontrôleurs et divers périphériques tels que les capteurs, les écrans LCD, etc.

Registres I2C :

TWBR (Two Wire Bit Rate Register) : Ce registre est utilisé pour définir la fréquence de l'I2C. Pour une fréquence de 100 kHz, vous pouvez calculer la valeur à mettre dans ce registre en utilisant la formule suivante : TWBR = (F_CPU / F_SCL - 16) / 2, où F_CPU est la fréquence du microcontrôleur et F_SCL est la fréquence de l'I2C.

TWSR (Two Wire Status Register) : Ce registre contient le statut de l'I2C après chaque opération. Vous pouvez le lire pour vérifier si l'opération a réussi ou non.

TWCR (Two Wire Control Register) : Ce registre est utilisé pour contrôler l'I2C. Vous pouvez l'utiliser pour démarrer ou arrêter une transmission, envoyer un accusé de réception, etc.

TWDR (Two Wire Data Register) : Ce registre contient les données à envoyer ou reçues.
*/