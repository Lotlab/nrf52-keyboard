SRC_FILES += \
	$(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \

CFLAGS += -DNRF52
CFLAGS += -DNRF52832_XXAA
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS

ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52832_XXAA

ifndef NRF52_DISABLE_FPU
	CFLAGS += -DFLOAT_ABI_HARD
	CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
	ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
	ASMFLAGS += -DFLOAT_ABI_HARD
	LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
else
	CFLAGS += -DFLOAT_ABI_SOFT
	CFLAGS += -mfloat-abi=soft
	ASMFLAGS += -mfloat-abi=soft
	ASMFLAGS += -DFLOAT_ABI_SOFT
endif

SOFTDEVICE := S132
HEAP_SIZE := 2048
STACK_SIZE := 2048

LD_NAME := nrf52832_s132.ld

NRF_FAMILY := NRF52
