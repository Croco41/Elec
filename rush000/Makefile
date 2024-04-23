#Nom du Projet
NAME=mainArduino

#info modele
DEVICE=atmega328p
F_CPU=16000000UL
BAUD_RATE=115200 #vitesse de transmission des données,nombre de symboles par seconde

#Configuration du compilateur
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS= -Os -DF_CPU=$(F_CPU) -mmcu=$(DEVICE)

#Configuration du port serie utilise (ls /dev/tty*)
PORT=/dev/ttyUSB0

#Fichiers Sources et objets
SRCS=main.c
OBJS=$(SRCS:.c=.o)

#hex:fichiers sources en binaires puis en hexadecimaux
#flash: televerse le fichier hexa sur le microcontroleur
all:	hex flash

#Compilation des fichiers source en objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Creation du fichier hex a partir des fichiers objets
#-O ihex spécifie le format de sortie désiré, ici, Intel Hex.
# Creation du fichier hex à partir des fichiers objets
hex: $(NAME).hex

$(NAME).bin: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(NAME).hex: $(NAME).bin
	$(OBJCOPY) -O ihex $< $@

#hex:	$(OBJS)
#	$(CC) $(CFLAGS) -o $(NAME).bin $(OBJS)
#	$(OBJCOPY) -O ihex $(NAME).bin $(NAME).hex

#Flash du microcontroleur
#avrdude:  programme pour téléverser et manipuler le firmware des microcontr. AVR
#mémoire:opération:fichier:format : 
flash:
	avrdude -c arduino -p ${DEVICE} -P $(PORT) -b $(BAUD_RATE) -U flash:w:$(NAME).hex:i

clean:
	rm -rf $(OBJS) $(NAME).bin $(NAME).hex

#Recompil
re:	clean all 

# Déclaration des cibles ne correspondant pas à des fichiers
.PHONY:	all hex flash clean re

