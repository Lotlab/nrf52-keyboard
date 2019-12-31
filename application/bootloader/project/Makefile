PROJECT_NAME     := secure_bootloader_ble_s112_pca10040e
TARGETS          := nrf52_bootloader

ifndef OUTPUT_DIRECTORY
OUTPUT_DIRECTORY := _build
endif

ifndef PROJ_DIR
PROJ_DIR := ..
endif

ifndef APP_PROJ_DIR
APP_PROJ_DIR := $(PROJ_DIR)/project
endif

ifndef ROOT_DIR
	ROOT_DIR := ../../..
endif

ifndef SDK_ROOT
	SDK_ROOT := $(ROOT_DIR)/SDK
endif

ifndef TEMPLATE_PATH
	TEMPLATE_PATH := $(ROOT_DIR)/template
endif

ifeq ($(NRF_CHIP), nrf52810)
	include $(APP_PROJ_DIR)/nrf52810.mk
else ifeq ($(NRF_CHIP), nrf52832)
	include $(APP_PROJ_DIR)/nrf52832.mk
else
	$(error cannot handle NRF_CHIP [$(NRF_CHIP)])
endif

$(OUTPUT_DIRECTORY)/nrf52_bootloader.out: \
	LINKER_SCRIPT  := $(APP_PROJ_DIR)/$(LD_NAME)

# Source files common to all targets
SRC_FILES += \
	$(SDK_ROOT)/components/libraries/util/app_error_weak.c \
	$(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
	$(SDK_ROOT)/components/libraries/util/app_util_platform.c \
	$(SDK_ROOT)/components/libraries/crc32/crc32.c \
	$(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c \
	$(SDK_ROOT)/components/libraries/util/nrf_assert.c \
	$(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
	$(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
	$(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
	$(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
	$(SDK_ROOT)/components/libraries/queue/nrf_queue.c \
	$(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
	$(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
	$(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
	$(SDK_ROOT)/components/libraries/sha256/sha256.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecc.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdh.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc/micro_ecc_backend_ecdsa.c \
	$(SDK_ROOT)/components/boards/boards.c \
	$(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
	$(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
	$(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
	$(PROJ_DIR)/dfu_public_key.c \
	$(PROJ_DIR)/main.c \
	$(SDK_ROOT)/components/ble/common/ble_srv_common.c \
	$(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader.c \
	$(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_app_start.c \
	$(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_app_start_final.c \
	$(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_dfu_timers.c \
	$(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_fw_activation.c \
	$(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_info.c \
	$(SDK_ROOT)/components/libraries/bootloader/nrf_bootloader_wdt.c \
	$(SDK_ROOT)/external/nano-pb/pb_common.c \
	$(SDK_ROOT)/external/nano-pb/pb_decode.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw/nrf_sw_backend_hash.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/dfu-cc.pb.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu.c \
	$(SDK_ROOT)/components/libraries/bootloader/ble_dfu/nrf_dfu_ble.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_handling_error.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_mbr.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_req_handler.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings_svci.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_transport.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_utils.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_validation.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_ver_validation.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
	$(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_svci_handler.c \
	$(SDK_ROOT)/components/libraries/svc/nrf_svc_handler.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
	$(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c \

# Include folders common to all targets
INC_FOLDERS += \
	$(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
	$(SDK_ROOT)/components/libraries/memobj \
	$(SDK_ROOT)/components/libraries/sha256 \
	$(SDK_ROOT)/components/libraries/crc32 \
	$(SDK_ROOT)/components/libraries/experimental_section_vars \
	$(SDK_ROOT)/components/libraries/mem_manager \
	$(SDK_ROOT)/components/libraries/fstorage \
	$(SDK_ROOT)/components/libraries/util \
	$(SDK_ROOT)/modules/nrfx \
	$(SDK_ROOT)/external/nrf_oberon/include \
	$(SDK_ROOT)/components/libraries/crypto/backend/oberon \
	$(SDK_ROOT)/components/libraries/crypto/backend/cifra \
	$(SDK_ROOT)/components/libraries/atomic \
	$(SDK_ROOT)/integration/nrfx \
	$(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
	$(SDK_ROOT)/components/libraries/log/src \
	$(SDK_ROOT)/components/libraries/bootloader/dfu \
	$(SDK_ROOT)/components/ble/common \
	$(SDK_ROOT)/components/libraries/delay \
	$(SDK_ROOT)/components/libraries/svc \
	$(SDK_ROOT)/components/libraries/stack_info \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
	$(SDK_ROOT)/components/libraries/log \
	$(SDK_ROOT)/external/nrf_oberon \
	$(SDK_ROOT)/components/libraries/strerror \
	$(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
	$(SDK_ROOT)/components/boards \
	$(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
	$(SDK_ROOT)/components/libraries/bootloader \
	$(SDK_ROOT)/components/libraries/crypto \
	$(SDK_ROOT)/components/libraries/crypto/backend/optiga \
	$(SDK_ROOT)/components/libraries/scheduler \
	$(SDK_ROOT)/modules/nrfx/hal \
	$(SDK_ROOT)/components/toolchain/cmsis/include \
	$(SDK_ROOT)/components/libraries/balloc \
	$(SDK_ROOT)/components/libraries/atomic_fifo \
	$(SDK_ROOT)/external/micro-ecc/micro-ecc \
	$(PROJ_DIR) \
	$(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
	$(SDK_ROOT)/modules/nrfx/mdk \
	$(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
	$(SDK_ROOT)/components/softdevice/common \
	$(SDK_ROOT)/external/nano-pb \
	$(SDK_ROOT)/components/libraries/queue \
	$(SDK_ROOT)/components/libraries/ringbuf \

# Libraries common to all targets
LIB_FILES += \
	$(SDK_ROOT)/external/nrf_oberon/lib/cortex-m4/soft-float/liboberon_2.0.7.a \
	$(SDK_ROOT)/external/micro-ecc/nrf52nf_armgcc/armgcc/micro_ecc_lib_nrf52.a \

# Optimization flags
OPT = -Os -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=2
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
CFLAGS += -DuECC_ENABLE_VLI_API=0
CFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
CFLAGS += -DuECC_SQUARE_FUNC=0
CFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
CFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRFX_COREDEP_DELAY_US_LOOP_CYCLES=3
ASMFLAGS += -DNRF_DFU_SETTINGS_VERSION=2
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=6
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
ASMFLAGS += -DuECC_ENABLE_VLI_API=0
ASMFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
ASMFLAGS += -DuECC_SQUARE_FUNC=0
ASMFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
ASMFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1

ifeq ($(SOFTDEVICE), S112)
	CFLAGS += -DS112
	ASMFLAGS += -DS112
	SOFTDEVICE_NAME := s112_nrf52_6.1.1_softdevice.hex
	SOFTDEVICE_VER  := 0xb8
	SOFTDEVICE_PATH := $(SDK_ROOT)/components/softdevice/s112/hex/s112_nrf52_6.1.1_softdevice.hex
	
    INC_FOLDERS += \
		$(SDK_ROOT)/components/softdevice/s112/headers/nrf52 \
		$(SDK_ROOT)/components/softdevice/s112/headers

else ifeq ($(SOFTDEVICE), S132)
	CFLAGS += -DS132
	ASMFLAGS += -DS132
	SOFTDEVICE_NAME := s132_nrf52_6.1.1_softdevice.hex
	SOFTDEVICE_VER  := 0xb7
	SOFTDEVICE_PATH := $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex
	
	INC_FOLDERS += \
		$(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
		$(SDK_ROOT)/components/softdevice/s132/headers

else
	$(error cannot handle softdevice [$(SOFTDEVICE)])
endif

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52_bootloader: CFLAGS += -D__HEAP_SIZE=$(HEAP_SIZE)
nrf52_bootloader: CFLAGS += -D__STACK_SIZE=$(STACK_SIZE)
nrf52_bootloader: ASMFLAGS += -D__HEAP_SIZE=$(HEAP_SIZE)
nrf52_bootloader: ASMFLAGS += -D__STACK_SIZE=$(STACK_SIZE)

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52_bootloader

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52_bootloader
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase pyocd_flash pyocd_flash_softdevice pyocd_erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52_bootloader.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52_bootloader.hex --sectorerase
	nrfjprog -f nrf52 --reset

pyocd_flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52_bootloader.hex
	pyocd flash -t nrf52 -e sector -f 2M $(OUTPUT_DIRECTORY)/nrf52_bootloader.hex
	pyocd cmd -t nrf52 -c reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: $(SOFTDEVICE_NAME)
	nrfjprog -f nrf52 --program $(SOFTDEVICE_PATH) --sectorerase
	nrfjprog -f nrf52 --reset

pyocd_flash_softdevice:
	@echo Flashing: $(SOFTDEVICE_NAME)
	pyocd flash -t nrf52 -e sector -f 2M $(SOFTDEVICE_PATH)
	pyocd cmd -t nrf52 -c reset

merge_softdevice: default
	@echo Merging program and SoftDevice $(SOFTDEVICE_NAME) to $(OUTPUT_DIRECTORY)/nrf52_bootloader_sd.hex
	mergehex -m $(SOFTDEVICE_PATH) $(OUTPUT_DIRECTORY)/nrf52_bootloader.hex -o $(OUTPUT_DIRECTORY)/nrf52_bootloader_sd.hex

erase:
	nrfjprog -f nrf52 --eraseall

pyocd_erase:
	pyocd erase -t nrf52 -c

SDK_CONFIG_FILE := ../sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
