OUTPUT_DIR = _build
SUBDIRS=$(filter-out $(OUTPUT_DIR),$(shell ls -l | grep ^d | grep -v 'ergoconn\|g84-4100\|lkb-core\|lot60-ble\|template\|test' | awk '{print $$9}'))
export VERSION=$(shell git describe --always --dirty || echo 'unknown')
export BUILDTIME=$(shell date +"%Y%m%d")
export FILE_VERSION = $(BUILDTIME)-$(VERSION)
export RELEASE_DIR = ../$(OUTPUT_DIR)

.PHONY: clean all dist_clean
all: $(SUBDIRS)

$(SUBDIRS): $(OUTPUT_DIR)
	@-mkdir $(OUTPUT_DIR)/$@
	@make -C $@ package OUTPUT_DIRECTORY=$(RELEASE_DIR)/$@ NRF_PACKAGE_NAME=$(RELEASE_DIR)/$@-nrf52-$(FILE_VERSION).zip BUILD_PREFIX=$@
	@make -C $@ usb OUTPUT_DIRECTORY=$(RELEASE_DIR)/$@ OUTPUT_BIN=$(RELEASE_DIR)/$@-ch554-$(FILE_VERSION).bin BUILD_PREFIX=$@
	@make -C $@ merge_setting OUTPUT_DIRECTORY=$(RELEASE_DIR)/$@ NRF_MERGE_SIGN_NAME=$(RELEASE_DIR)/$@-nrf52_kbd_sign.hex BUILD_PREFIX=$@
	@make -C $@ make_all OUTPUT_DIRECTORY=$(RELEASE_DIR)/$@ NRF_MERGE_ALL_NAME=$(RELEASE_DIR)/$@-$(BUILDTIME)-nrf52-all.hex BUILD_PREFIX=$@
	
$(OUTPUT_DIR):
	@-mkdir -p $(OUTPUT_DIR)

dist_clean: 
	-rm -r $(OUTPUT_DIR)/*/

clean:
	-rm -rf $(OUTPUT_DIR)
