ifndef BOOTLOADER_DIR
	BOOTLOADER_DIR := $(ROOT_DIR)/application/bootloader/project
endif

export NRF_CHIP SOFTDEVICE NRF52_DISABLE_FPU CONFIG_H

.PHONY: bootloader

BL_MAKE_ARGS = -C $(BOOTLOADER_DIR)  

ifdef OUTPUT_DIRECTORY
BL_MAKE_ARGS += OUTPUT_DIRECTORY=$(abspath $(OUTPUT_DIRECTORY)) 
endif
ifdef CONFIG_H_DIR
BL_MAKE_ARGS += CONFIG_H_DIR=$(abspath $(CONFIG_H_DIR)) 
endif

ifndef BOOTLOADER_VERSION
	BOOTLOADER_VERSION := 1
endif

bootloader: 
	@make $(BL_MAKE_ARGS)

flash_bootloader:
	@make $(BL_MAKE_ARGS) flash

pyocd_flash_bootloader:
	@make $(BL_MAKE_ARGS) pyocd_flash

# Package bootloader DFU pack
package_bl: bootloader
	@echo Packing: $(OUTPUT_DIRECTORY)/nrf52_bootloader.zip
	nrfutil pkg generate --hw-version 52 --bootloader-version $(BOOTLOADER_VERSION) --bootloader $(OUTPUT_DIRECTORY)/nrf52_bootloader.hex \
	--sd-req $(SOFTDEVICE_VER) --key-file $(APP_PROJ_DIR)/private.key $(OUTPUT_DIRECTORY)/nrf52_bootloader.zip
