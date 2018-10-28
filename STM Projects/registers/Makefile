CC = arm-none-eabi-gcc

INCLUDE = -Iinclude -Iinclude/CMSIS
LINKER = $(INCLUDE)/STM32F091RCTx_FLASH.ld
COMMON_FLAGS = -std=c99 -mcpu=cortex-m0 -mlittle-endian -mthumb -DSTM32F091xC
COMP_FLAGS = $(COMMON_FLAGS) -g3 -O0 -Wall -Wextra $(INCLUDE) -c -S
LINK_FLAGS = $(COMMON_FLAGS) -T$(LINKER) -Wl,--gc-sections

OOCD_DIR = /usr/share/openocd
OOCD_SCRIPTS = $(OOCD_DIR)/scripts
OOCD_CFG = $(OOCD_SCRIPTS)/board/st_nucleo_f0.cfg
OOCD = openocd

GDB = arm-none-eabi-gdb
OBJCOPY = arm-none-eabi-objcopy


STARTUP = $(INCLUDE)/startup_stm32f091xc.s
SYSTEM = $(INCLUDE)/system_stm32f0xx.c
SOURCE = src/main.c

DEST = dest
SOURCE_O = $(DEST)/main.o
UTILS_O = $(DEST)/utils.o
STARTUP_O = $(DEST)/startup.o
SYSTEM_O = $(DEST)/system.o

OUT_ELF = $(DEST)/main.elf
OUT_HEX = $(DEST)/main.hex

.PHONY: default compile con debug flash

default: compile con
	compile
	con

compile:
	$(CC) $(COMP_FLAGS) $(SOURCE) -o $(SOURCE_O)
	$(CC) $(COMP_FLAGS) $(STARTUP) -o $(STARTUP_O)
	$(CC) $(COMP_FLAGS) $(SYSTEM) -o $(SYSTEM_O)	
	$(CC) $(COMP_FLAGS) src/common.c -o dest/common.o
	$(CC) $(COMP_FLAGS) src/configure.c -o dest/configure.o
	
	$(CC) $(LINK_FLAGS) $(SOURCE_O)  $(STARTUP_O) $(SYSTEM_O) dest/common.o dest/configure.o dest/trig.o -o $(OUT_ELF)
	$(OBJCOPY) -Oihex $(OUT_ELF) $(OUT_HEX)

con:
	$(OOCD) -s $(OOCD_SCRIPTS) -f $(OOCD_CFG)

debug:
	$(GDB) -x scripts/debug.gdb

flash:
	$(GDB) -x scripts/flash.gdb

