SRC_FILES += \
	$(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \

CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52
CFLAGS += -DNRF52832_XXAA
CFLAGS += -DS132
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52832_XXAA

LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

SOFTDEVICE := S132
HEAP_SIZE := 2048
STACK_SIZE := 2048

LD_NAME := nrf52832_s132.ld
