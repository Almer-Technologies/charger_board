# Makefile to build software for atmega328p
# Iacopo Sprenger

# SanpellegrinOS

TARGET=sanpellegrinos

#parameters

TOOLCHAIN=../avr8-gnu-toolchain-linux_x86_64/bin

MCU=atmega328p

CPU_FREQ=16000000UL

OPT=s

FORMAT=ihex

DEBUG_LEVEL=

WARNINGS=#-Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused


#tools

SHELL = sh

CC = $(TOOLCHAIN)/avr-gcc

OBJCOPY = $(TOOLCHAIN)/avr-objcopy
OBJDUMP = $(TOOLCHAIN)/avr-objdump
SIZE = $(TOOLCHAIN)/avr-size

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
HEADERDIR = inc

ASOURCES = $(wildcard $(SOURCEDIR)/*.s)
CSOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS =  $(patsubst $(SOURCEDIR)/%.s, $(BUILDDIR)/%.o, $(ASOURCES))
OBJECTS += $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(CSOURCES)) 
 
CURR_DIR = $(notdir $(shell pwd))

COLOR_START="\x1b[1;34m"
COLOR_STOP="\x1b[0m"
SAY_BUILD=${COLOR_START}"[build]"${COLOR_STOP}


all: clean $(TARGET).elf $(TARGET).hex $(TARGET).lst size

$(TARGET).elf: $(OBJECTS)
	@/bin/echo -e ${SAY_BUILD}" compiling elf file: " $^
	${CC} -mmcu=${MCU} $(CFLAGS) -o $@ $^



size: $(TARGET).elf
	@/bin/echo -e ${SAY_BUILD}" final size: "
	${SIZE} $<


%.hex: %.elf
	@/bin/echo -e ${SAY_BUILD}" copying binary: " $<
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.lst: %.elf
	@/bin/echo -e ${SAY_BUILD}" dumping listing: " $<
	$(OBJDUMP) -h -S $< > $@

# Compile: create object files from C source files.
$(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	@/bin/echo -e ${SAY_BUILD}" compiling objects from c source: " $<
	$(CC) -c -I$(HEADERDIR) -I$(SOURCEDIR) $(CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
$(BUILDDIR)/%.o : $(SOURCEDIR)/%.s
	@/bin/echo -e ${SAY_BUILD}" compiling objects from s source: " $<
	$(CC) -c -I$(HEADERDIR) -I$(SOURCEDIR) $(CFLAGS) $< -o $@

clean:
	$(REMOVE) build/*
	$(REMOVE) *.elf *.hex *.lst

program: clean $(TARGET).elf $(TARGET).hex $(TARGET).lst
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)


