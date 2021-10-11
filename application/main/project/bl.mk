ifndef BOOTLOADER_DIR
	BOOTLOADER_DIR := $(ROOT_DIR)/application/bootloader/project
endif

export NRF_CHIP SOFTDEVICE NRF52_DISABLE_FPU CONFIG_H

.PHONY: bootloader

BL_MAKE_ARGS = -C $(BOOTLOADER_DIR)  

ifdef OUTPUT_DIRECTORY
BL_MAKE_ARGS += OUTPUT_DIRECTORY=$(shell realpath --relative-to $(BOOTLOADER_DIR) $(OUTPUT_DIRECTORY))
endif
ifdef CONFIG_H_DIR
BL_MAKE_ARGS += CONFIG_H_DIR=$(shell realpath --relative-to $(BOOTLOADER_DIR) $(CONFIG_H_DIR))
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

merge_bootloader_all: bootloader merge_all
	@echo Merging program, signature, SoftDevice and bootloader to $(OUTPUT_DIRECTORY)/nrf52_all.hex
	mergehex -m $(OUTPUT_DIRECTORY)/nrf52_bootloader.hex $(OUTPUT_DIRECTORY)/nrf52_kbd_sign_with_sd.hex -o $(OUTPUT_DIRECTORY)/nrf52_all.hex

