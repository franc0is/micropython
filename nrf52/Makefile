include ../py/mkenv.mk

CROSS = 0

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

ifeq ($(CROSS), 1)
CROSS_COMPILE = arm-none-eabi-
endif

INC += -I.
INC += -I..
INC += -I../lib/mp-readline
INC += -I$(BUILD)
INC += -Icomponents
INC += -Icomponents/libraries/util
INC += -Icomponents/toolchain/gcc
INC += -Icomponents/drivers_nrf/uart
INC += -Icomponents/drivers_nrf/common
INC += -Icomponents/libraries/fifo
INC += -Icomponents/libraries/bsp
INC += -Icomponents/drivers_nrf/nrf_soc_nosd
INC += -Icomponents/toolchain
INC += -Icomponents/libraries/uart
INC += -Icomponents/device
INC += -Icomponents/libraries/log
INC += -Icomponents/boards
INC += -Icomponents/drivers_nrf/delay
INC += -Icomponents/toolchain/cmsis/include
INC += -Icomponents/drivers_nrf/hal
INC += -Icomponents/libraries/log/src

ifeq ($(CROSS), 1)
DFU = ../tools/dfu.py
PYDFU = ../tools/pydfu.py
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS = $(INC) -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)
else
CFLAGS = -m32 $(INC) -Wall -Werror -ansi -std=gnu99 $(COPT)
endif

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -Og -g -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

ifeq ($(CROSS), 1)
LDFLAGS = -nostdlib -g -T nrf52840_xxaa.ld -Map=$@.map --cref
else
LD = gcc
LDFLAGS = -m32 -Wl,-Map=$@.map,--cref
endif
LIBS =

#NRF52 SDK
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DBOARD_PCA10056
CFLAGS += -DBSP_DEFINES_ONLY
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -D__START=main
CFLAGS += -D__STARTUP_CLEAR_BSS

SRC_C = \
	main.c \
	uart_core.c \
	system_nrf52840.c \
	components/libraries/log/src/nrf_log_backend_serial.c \
	components/libraries/log/src/nrf_log_frontend.c \
	components/boards/boards.c \
	components/libraries/util/app_error.c \
	components/libraries/util/app_error_weak.c \
	components/libraries/fifo/app_fifo.c \
	components/libraries/uart/app_uart_fifo.c \
	components/libraries/util/app_util_platform.c \
	components/libraries/util/nrf_assert.c \
	components/libraries/uart/retarget.c \
	components/libraries/util/sdk_errors.c \
	components/drivers_nrf/common/nrf_drv_common.c \
	components/drivers_nrf/uart/nrf_drv_uart.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(PY_O) $(addprefix $(BUILD)/, gcc_startup_nrf52840.o)

ifeq ($(CROSS), 1)
all: $(BUILD)/firmware.dfu
else
all: $(BUILD)/firmware.elf
endif

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)../tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin

$(BUILD)/firmware.dfu: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(DFU) -b 0x08000000:$(BUILD)/firmware.bin $@

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<

# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include ../py/mkrules.mk
