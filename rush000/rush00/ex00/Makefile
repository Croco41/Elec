# SETTINGS
MCU = atmega328p
PORT = /dev/ttyUSB0
PORT2 = /dev/ttyUSB1
PROGRAMMER = arduino
F_CPU = 16000000UL
UART_BAUDRATE = 115200

# Options de compilation
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
CFLAGS = -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(UART_BAUDRATE) -I ./inc

# Fichiers source
SRC = main.c uart.c i2c.c timer.c master.c slave.c led.c
SRC := $(addprefix src/, $(SRC))
OBJ = $(SRC:.c=.o)

all: kill hex flash screen

hex: main.hex

main.hex: main.bin
	$(OBJCOPY) -O ihex -R .eeprom $< $@

main.bin: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

flash:
	$(AVRDUDE) -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -b $(UART_BAUDRATE) -U flash:w:main.hex
	$(AVRDUDE) -p $(MCU) -P $(PORT2) -c $(PROGRAMMER) -b $(UART_BAUDRATE) -U flash:w:main.hex

screen:
	gnome-terminal -- screen $(PORT) $(UART_BAUDRATE)
	gnome-terminal -- screen $(PORT2) $(UART_BAUDRATE)

kill:
	killall screen &> /dev/null || true

clean:
	rm -f main.hex main.bin $(OBJ)

re: clean all

.PHONY: all hex flash clean re screen

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@