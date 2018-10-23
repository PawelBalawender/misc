OOCD_DIR = /usr
OOCD_SCRIPTS = $(OOCD_DIR)/share/openocd/scripts
#OOCD = $(OOCD_DIR)/bin/openocd.exe
OOCD = openocd
OOCD_CFG = $(OOCD_SCRIPTS)/board/st_nucleo_f0.cfg

ARM_NONE_EABI_GCC = arm-none-eabi-gcc
ARM_NONE_EABI_GDB = arm-none-eabi-gdb
ARM_NONE_EABI_OBJCOPY = arm-none-eabi-objcopy

INCLUDE = include
CMSIS_CORE = $(INCLUDE)/CMSIS

LINKER = $(INCLUDE)/STM32F091RCTx_FLASH.ld
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

flash:
	$(ARM_NONE_EABI_GDB) -x scripts/flash.gdb

compile:
	$(ARM_NONE_EABI_GCC) -std=c99 -g3 -O0 -Wall -Wextra -mcpu=cortex-m0 -mlittle-endian -mthumb -I$(INCLUDE) -I$(CMSIS_CORE) -DSTM32F091xC -c $(SOURCE) -o $(SOURCE_O)
	$(ARM_NONE_EABI_GCC) -std=c99 -g3 -O0 -Wall -Wextra -mcpu=cortex-m0 -mlittle-endian -mthumb -I$(INCLUDE) -I$(CMSIS_CORE) -DSTM32F091xC -c $(STARTUP) -o $(STARTUP_O)
	$(ARM_NONE_EABI_GCC) -std=c99 -g3 -O0 -Wall -Wextra -mcpu=cortex-m0 -mlittle-endian -mthumb -I$(INCLUDE) -I$(CMSIS_CORE) -DSTM32F091xC -c $(SYSTEM) -o $(SYSTEM_O)	
	$(ARM_NONE_EABI_GCC) -std=c99 -g3 -O0 -Wall -Wextra -mcpu=cortex-m0 -mlittle-endian -mthumb -I$(INCLUDE) -I$(CMSIS_CORE) -DSTM32F091xC -c src/common.c -o dest/common.o
	$(ARM_NONE_EABI_GCC) -std=c99 -g3 -O0 -Wall -Wextra -mcpu=cortex-m0 -mlittle-endian -mthumb -I$(INCLUDE) -I$(CMSIS_CORE) -DSTM32F091xC -c src/configure.c -o dest/configure.o
	
	$(ARM_NONE_EABI_GCC) -std=c99 -mcpu=cortex-m0 -mlittle-endian -mthumb -DSTM32F091xC -T$(LINKER) -Wl,--gc-sections $(SOURCE_O)  $(STARTUP_O) $(SYSTEM_O) dest/common.o dest/configure.o dest/trig.o -o $(OUT_ELF)
	$(ARM_NONE_EABI_OBJCOPY) -Oihex $(OUT_ELF) $(OUT_HEX)

con:
	$(OOCD) -s $(OOCD_SCRIPTS) -f $(OOCD_CFG)

debug:
	$(ARM_NONE_EABI_GDB) -x scripts/debug.gdb
