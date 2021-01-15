OUTPUT_DIR = _build
SUBDIRS=$(filter-out _build common,$(shell ls -l | grep ^d | awk '{print $$9}'))
export VERSION=$(shell git describe --always --dirty || echo 'unknown')
export BUILDTIME=$(shell date +"%Y_%m_%d")
export FILE_VERSION = $(BUILDTIME)-$(VERSION)

ifdef OUTPUT_DIRECTORY
OUTPUT_DIR = $(OUTPUT_DIRECTORY)
endif
ifndef RELEASE_DIR
RELEASE_DIR = $(OUTPUT_DIR)
endif

.PHONY: clean all package usb merge_setting dist_clean

package: all
usb: 
	@echo use make all to build your release firmware.
merge_setting: 
	@echo use make all to build your release firmware.

all: $(SUBDIRS)

$(SUBDIRS): $(OUTPUT_DIR)
	@-mkdir -p $(OUTPUT_DIR)/$@
	@make -C $@ package OUTPUT_DIRECTORY=../$(OUTPUT_DIR)/$@ NRF_PACKAGE_NAME=../$(RELEASE_DIR)/$(BUILD_PREFIX)-$@-nrf52-$(FILE_VERSION).zip BUILD_PREFIX=$(BUILD_PREFIX)-$@
	@make -C $@ usb OUTPUT_DIRECTORY=../$(OUTPUT_DIR)/$@ OUTPUT_BIN=../$(RELEASE_DIR)/$(BUILD_PREFIX)-$@-ch554-$(FILE_VERSION).bin BUILD_PREFIX=$(BUILD_PREFIX)-$@
	@make -C $@ merge_setting OUTPUT_DIRECTORY=../$(OUTPUT_DIR)/$@ NRF_SIGN_HEX_NAME=../$(RELEASE_DIR)/$(BUILD_PREFIX)-$@-nrf52-$(FILE_VERSION).hex BUILD_PREFIX=$(BUILD_PREFIX)-$@

$(OUTPUT_DIR):
	@-mkdir -p $(OUTPUT_DIR)

dist_clean: 
	-rm -r $(OUTPUT_DIR)/*/

clean:
	-rm -r $(OUTPUT_DIR)