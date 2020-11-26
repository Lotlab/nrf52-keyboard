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
	
# Libraries common to all targets
LIB_FILES += \
	$(SDK_ROOT)/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_2.0.7.a \
	$(SDK_ROOT)/external/micro-ecc/nrf52hf_armgcc/armgcc/micro_ecc_lib_nrf52.a
else
	CFLAGS += -DFLOAT_ABI_SOFT
	CFLAGS += -mfloat-abi=soft
	ASMFLAGS += -mfloat-abi=soft
	ASMFLAGS += -DFLOAT_ABI_SOFT
	
# Libraries common to all targets
LIB_FILES += \
	$(SDK_ROOT)/external/nrf_oberon/lib/cortex-m4/soft-float/liboberon_2.0.7.a \
	$(SDK_ROOT)/external/micro-ecc/nrf52nf_armgcc/armgcc/micro_ecc_lib_nrf52.a
endif

SOFTDEVICE := S132
HEAP_SIZE := 2048
STACK_SIZE := 2048

LD_NAME := nrf52832_s132.ld

NRF_FAMILY := NRF52
