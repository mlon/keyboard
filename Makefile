BUILD := debug

TARGET = ix37
AVRDUDE_PROGRAMMER = atmelice_isp

MCU = atmega328p
F_CPU = 16000000UL
FORMAT = ihex

BUILDDIR = build/${BUILD}
SOURCEDIR = src
SOURCES = $(wildcard ${SOURCEDIR}/*.c)
OBJECTS=${patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,${SOURCES}}

CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -Wa,-adhlns=$(<:$(SOURCEDIR)/%.c=$(BUILDDIR)/%.lst)
CFLAGS += -std=gnu11
CFLAGS += -DF_CPU=$(F_CPU)

ifeq ($(BUILD), debug)
	CFLAGS += -Og -ggdb3
else
	CFLAGS += -Os
endif

TARGET_ARCH = -mmcu=$(MCU)

LDFLAGS = -Wl,-Map=$(BUILDDIR)/$(TARGET).map,--cref -Wl,--gc-sections 

LFUSE = 0xFF
HFUSE = 0xDA
HFUSE_DBG = 0x9A
EFUSE = 0xF7


CC = avr-gcc
LD = avr-gcc
OBJCOPY = avr-objcopy


all: $(BUILDDIR)/$(TARGET).hex
 
flash: $(BUILDDIR)/$(TARGET).hex
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(MCU) -v -U flash:w:$(BUILDDIR)/$(TARGET).hex

fuses: 
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(MCU) -v -U efuse:w:$(EFUSE):m -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

enable_dw:
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(MCU) -v -U hfuse:w:$(HFUSE_DBG):m

disable_dw:
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(MCU) -v -U hfuse:w:$(HFUSE):m

erase: 
	avrdude -c $(AVRDUDE_PROGRAMMER) -p $(MCU) -v -e
 
$(BUILDDIR)/%.hex: $(BUILDDIR)/%.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
 
$(BUILDDIR)/%.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@

$(OBJECTS): $(BUILDDIR)/%.o: ${SOURCEDIR}/%.c | $(BUILDDIR)
	$(CC) $(TARGET_ARCH) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)
 
clean: 
	rm -fr $(BUILDDIR)

.PHONY: fuses clean
.PRECIOUS: $(BUILDDIR)/%.elf