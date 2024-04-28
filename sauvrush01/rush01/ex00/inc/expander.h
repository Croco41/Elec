#ifndef EXPANDER_H
#define EXPANDER_H

#include <avr/io.h>
#include <util/twi.h>

#define PCA9555_ADDRESS 0x20 // Adresse esclave du PCA9555PW

#define PCA_CNF0_REG 0x06 // Registre de configuration du port 0 (100_3 lie au port 0)
#define PCA_OUT0_REG 0x02 // Registre de sortie du port 0
#define PCA_IN0_REG 0x00  // en entree

#define I2C_READ 1
#define I2C_WRITE 0

void exp_transmit_cmd(uint8_t *cmd, uint8_t nb_octets);

void init_nbrs();
void setup_pca9555();
void turn_on_nbr(uint8_t n, uint8_t c);
void turn_off_nbrs();
void manual_turn_on(uint8_t a, uint8_t b);

void startRush(void);

#endif