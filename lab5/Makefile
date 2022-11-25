# Назва проекту
PROJECT_NAME = lab5
# Модель мікроконтролера
MCU = atmega328p
# Частота тактового генератора
F_CPU = 8000000UL
# Порт для програмування
UPLOAD_PORT=/dev/ttyUSB0
# Шридкість передачі даних [19200 (nano atmega168p), 57600 (nano atmega328p), 115200 (arduino uno), ...]
UPLOAD_PORT_BAUD=57600

# Абсолютний шлях до програми-програматора 
AVRDUDE = avrdude
AVRDUDE_CONF = /etc/avrdude.conf

# Sources files needed for building the application 
# SRC = $(wildcard *.c)
# SRC = main.c
SRC = main.c
SRC +=

# The headers files needed for building the application
# INCLUDE = -I. 
INCLUDE = -I.
INCLUDE +=

#--------------------------------------------------------------
PROJECT_NAME := $(strip $(PROJECT_NAME))
THIS_DIR := $(dir $(abspath $(firstword $(MAKEFILE_LIST))))

#OBJS:=$(SRC: .c=.o)
OBJS:= $(addsuffix .o,$(basename $(SRC)))
OBJS:= $(addprefix build\,$(OBJS))

ifeq ($(OS),Windows_NT)
	TARGET_DIR = $(subst /,\,$(THIS_DIR))build
	RM=del
else
	TARGET_DIR = $(TARGET_DIR)/build
	RM=rm -rf
endif

CC = avr-gcc
OBJCOPY = avr-objcopy

all: $(TARGET_DIR) clean build\$(PROJECT_NAME).hex sizedummy

build\$(PROJECT_NAME).hex: build\$(PROJECT_NAME).elf
	@echo Create exec file $@
	@$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo BUILD SUCCESS!
	
build\$(PROJECT_NAME).elf: $(OBJS)	
	@echo Building target: $@
	@$(CC) -Os -mmcu=$(MCU) -o build\$(PROJECT_NAME).elf $^

build\\%.o: %.c # | $(TARGET_DIR)
	@echo Compile file: $<
	@$(CC) -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=$(F_CPU) -o $@ -c $<

$(TARGET_DIR):
	mkdir $(TARGET_DIR)

.PHONY: all clean sizedummy

sizedummy:
	@echo
	@avr-size --format=avr --mcu=$(MCU) build/$(PROJECT_NAME).elf
	
flash:
	@$(AVRDUDE) -C$(AVRDUDE_CONF) -F -v -p$(MCU) -carduino -P$(UPLOAD_PORT) -b$(UPLOAD_PORT_BAUD) -D -Uflash:w:build\$(PROJECT_NAME).hex:i

clean:
#	@$(RM) build\*.o build\$(PROJECT_NAME).elf build\$(PROJECT_NAME).hex
