TARGET		= libperiph.a
MCU			= msp430g2553

UTILS_ROOT  = /Users/chris/Projects/mcu_utils/ti/msp430-gcc
CC			= $(UTILS_ROOT)/bin/msp430-elf-gcc
GDB			= $(UTILS_ROOT)/bin/msp430-elf-gdb
AR			= $(UTILS_ROOT)/bin/msp430-elf-ar

SRCDIR		= ./src
BUILDDIR	= ./build
CFLAGS		= -I$(UTILS_ROOT)/include -I$(SRCDIR) -g -Wall -pedantic -Wno-main -ffunction-sections -fdata-sections
LDFLAGS		= -mmcu=$(MCU) -L$(UTILS_ROOT)/include

CSRC		= $(shell find $(SRCDIR) -iname "*.c")
COBJ		= $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(CSRC))

.PHONY: all

all: $(TARGET)

$(TARGET): $(COBJ)
	$(AR) rcs $(TARGET) $(COBJ)
	# $(CC) $(CFLAGS) $(COBJ) -o $(TARGET) $(LDFLAGS)

$(COBJ): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILDDIR) $(TARGET)
