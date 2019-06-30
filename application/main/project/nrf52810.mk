SRC_FILES += \
	$(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52810.S \
	$(SDK_ROOT)/modules/nrfx/mdk/system_nrf52810.c

CFLAGS += -DFLOAT_ABI_SOFT
CFLAGS += -DNRF52810_XXAA
CFLAGS += -mfloat-abi=soft

ASMFLAGS += -mfloat-abi=soft
ASMFLAGS += -DFLOAT_ABI_SOFT
ASMFLAGS += -DNRF52810_XXAA

SOFTDEVICE := S112
HEAP_SIZE := 2048
STACK_SIZE := 2048

LD_NAME := nrf52810_s112.ld

NRF_FAMILY := NRF52810
