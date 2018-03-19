##
##  This file is part of cm4-scheduler.
##
##  Copyright (C) 2018 Sean Chiang <n2i911@gmail.com>
##
##  cm4-scheduler is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  cm4-scheduler is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with os.h.  If not, see <http://www.gnu.org/licenses/>.
##

# Project name
TARGET = cm4-scheduler

TOOLCHAIN_PREFIX = arm-none-eabi-
CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(TOOLCHAIN_PREFIX)as
LD = $(TOOLCHAIN_PREFIX)gcc
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy

OUT_DIR = ./build
SRC_DIR = src

INC = -I./lib/Drivers/CMSIS/Include \
      -I./lib/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
      -I./lib/Projects/STM32F4-Discovery/Templates/Inc \
      -I./lib/Drivers/STM32F4xx_HAL_Driver/Inc \
      -I./include

# ASM and C (e.g. startup) source files and linker script outside src directory

SRC = $(wildcard $(addsuffix /*.c, $(SRC_DIR)/kernel)) \
      $(wildcard $(addsuffix /*.s, $(SRC_DIR)/kernel)) \
      $(wildcard $(addsuffix /*.c, $(SRC_DIR)/platform)) \
      $(wildcard $(addsuffix /*.s, $(SRC_DIR)/platform)) \
      ./lib/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
      ./lib/Projects/STM32F4-Discovery/Templates/Src/system_stm32f4xx.c \
      ./lib/Projects/STM32F4-Discovery/Templates/SW4STM32/startup_stm32f407xx.s

LD_SCRIPT = ./lib/Projects/STM32F4-Discovery/Templates/SW4STM32/STM32F4-Discovery/STM32F407VGTx_FLASH.ld

CPU = -mcpu=cortex-m4
FPU = -mfloat-abi=softfp \
      -mfpu=fpv4-sp-d16

# Defines required by included libraries
DEFS = -DSTM32F407xx

FLAGS = $(CPU) \
	-mthumb \
	-march=armv7e-m \
	-mlittle-endian \
	-mthumb-interwork \
	$(FPU)

# CFLAGS
CFLAGS = -fno-common \
	-ffreestanding \
	-O0 \
	-gdwarf-2 \
	-g3 \
	-Wall \
	-Werror \
	-std=c11 \
	$(FLAGS) \
	$(INC) \
	$(DEFS)

# LDFLAGS
LDFLAGS = -T $(LD_SCRIPT) $(FLAGS) -nostartfiles -specs=nosys.specs

OBJ := $(addprefix $(OUT_DIR)/,$(patsubst %.s, %.o, $(SRC:.c=.o)))

all: $(OUT_DIR)/$(TARGET).bin $(OUT_DIR)/$(TARGET).lst

$(OUT_DIR)/$(TARGET).bin: $(OUT_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(OUT_DIR)/$(TARGET).lst: $(OUT_DIR)/$(TARGET).elf
	$(OBJDUMP) -S $< > $@

$(OUT_DIR)/$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) -Wl,-Map=$(OUT_DIR)/$(TARGET).map -o $@ $^

$(OUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $<

$(OUT_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ -c $<
clean:
	rm -rf $(OUT_DIR)
