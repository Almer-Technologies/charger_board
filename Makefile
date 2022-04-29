# Makefile to build software for atmega328p
# Iacopo Sprenger

# SanpellegrinOS

TARGET=sanpellegrinos

#parameters

MCU=atmega328p

CPU_FREQ=16000000UL

OPT=s

FORMAT=ihex

DEBUG_LEVEL=-g

WARNINGS=-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused


#tools

SHELL = sh

CC = avr-gcc

OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

AVRDUDE=avrdude

REMOVE = rm -f
COPY = cp

HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf




#programmer

AVRDUDE_PROGRAMMER = arduino

AVRDUDE_PORT = /dev/ttyUSB0

AVRDUDE_WRITE_FLASH =-DV -U flash:w:$(TARGET).hex:i

AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -b 115200






CFLAGS=-O$(OPT) $(DEBUG_LEVEL) -DF_CPU=$(CPU_FREQ) -mmcu=$(MCU)\
$(WARNINGS)




BUILDDIR = build
SOURCEDIR = src
HEADERDIR = src

SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))



all: $(TARGET).elf $(TARGET).hex

$(TARGET).elf: $(OBJECTS)
	${CC} -mmcu=${MCU} -o $@ $^


%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

# Compile: create object files from C source files.
$(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) -c $(CFLAGS) -I$(HEADERDIR) -I$(SOURCEDIR) $< -o $@

# Compile: create assembler files from C source files.
$(BUILDDIR)/%.s : $(SOURCEDIR)/%.c
	$(CC) -S $(CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
$(BUILDDIR)/%.o : $(SOURCEDIR)/%.S
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(REMOVE) build/*
	$(REMOVE) *.elf *.hex

program: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


