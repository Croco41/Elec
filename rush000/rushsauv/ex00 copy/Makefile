# SETTINGS
MCU = atmega328p
PORT = /dev/ttyUSB0
PROGRAMMER = arduino
F_CPU = 16000000UL
UART_BAUDRATE = 115200

# Options de compilation
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
CFLAGS = -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(UART_BAUDRATE) -I ./inc

# Fichiers source
SRC = main.c uart.c i2c.c
SRC := $(addprefix src/, $(SRC))
OBJ = $(SRC:.c=.o)

all: hex flash screen

hex: main.hex

main.hex: main.bin
	$(OBJCOPY) -O ihex -R .eeprom $< $@

main.bin: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

flash:
	$(AVRDUDE) -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -b $(UART_BAUDRATE) -U flash:w:main.hex

screen:
	screen $(PORT) $(UART_BAUDRATE)

clean:
	rm -f main.hex main.bin $(OBJ)

re: clean all

.PHONY: all hex flash clean re screen

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@