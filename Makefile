ROOT_DIR := .
APP_SRC_DIR := $(ROOT_DIR)/src
USB_SRC_DIR := $(ROOT_DIR)/usb

TARGET_DIR = keyboard/$(TARGET)
BUILD_DIR = $(TARGET_DIR)/_build
USB_BUILD_DIR = $(TARGET_DIR)/_build_usb

DIST_DIR := _build

default_action: all

item_build : $(BUILD_DIR)/Makefile
	@echo Build 
	${MAKE} -C $(BUILD_DIR)

usb_build : $(USB_BUILD_DIR)/Makefile
	@echo Build 
	${MAKE} -C $(USB_BUILD_DIR)

item_setup : $(BUILD_DIR)/Makefile

$(BUILD_DIR)/Makefile : ${APP_SRC_DIR}/CMakeLists.txt item_check
	if [ ! -d $(BUILD_DIR) ] ; then mkdir $(BUILD_DIR) ; fi
	cd $(BUILD_DIR);  \
		cmake \
		    -G "Unix Makefiles" \
			-DEXTERNAL_INC_DIR=$(abspath $(TARGET_DIR)) \
			-DEXTERNAL_CMAKE_FILE=$(abspath $(TARGET_DIR)/rules.cmake) \
			$(abspath ${APP_SRC_DIR})

$(USB_BUILD_DIR)/Makefile : ${USB_SRC_DIR}/CMakeLists.txt item_check
	if [ ! -d $(USB_BUILD_DIR) ] ; then mkdir $(USB_BUILD_DIR) ; fi
	cd $(USB_BUILD_DIR);  \
		cmake \
		    -G "Unix Makefiles" \
			-DEXTERNAL_INC_DIR=$(abspath $(TARGET_DIR)) \
			-DEXTERNAL_CMAKE_FILE=$(abspath $(TARGET_DIR)/rules.cmake) \
			$(abspath ${USB_SRC_DIR})

item_clean: item_check
	rm -rf $(BUILD_DIR)

item_check:
ifeq ($(TARGET),)
	$(error 请使用 TARGET 环境变量指定编译的目标)
endif
ifeq ($(wildcard $(TARGET_DIR)/.),)
	$(error 目录 $(TARGET_DIR) 不存在)
endif
ifeq ($(wildcard $(TARGET_DIR)/rules.cmake),)
	$(error 配置文件 $(TARGET_DIR)/rules.cmake 不存在)
endif

SUBDIRS := $(wildcard keyboard/*/rules.cmake)
SUBTARGETS := $(SUBDIRS:keyboard/%/rules.cmake=%)
SUBTARGETS_CLEAN = $(SUBTARGETS:%=%_clean)
VERSION := $(shell git describe --always --dirty)

$(DIST_DIR): 
	mkdir $(DIST_DIR)

all: $(SUBTARGETS)
$(SUBTARGETS): $(DIST_DIR)
	@$(MAKE) TARGET=$@ item_build
	-cp keyboard/$@/_build/main.bin $(DIST_DIR)/$@-$(VERSION).bin
	-cp keyboard/$@/_build/main.hex $(DIST_DIR)/$@-$(VERSION).hex
	-cp keyboard/$@/_build/main.zip $(DIST_DIR)/$@-$(VERSION).zip
# todo: 真实的复制

help:
	@echo "following command avaliable: "
	@echo "all - build all keyboards and put them to $(DIST_DIR)"
	@echo "clean - clean all "
	@echo "KEYBOARD - build specific keyboard"
	@echo "KEYBOARD_clean - clean specific keyboard build"
	@echo ""
	@echo "keyboards: $(SUBTARGETS)"

clean_dist:
	-@rm -rf $(DIST_DIR)

clean: $(SUBTARGETS_CLEAN)
$(SUBTARGETS_CLEAN): clean_dist
	@$(MAKE) TARGET=${@:%_clean=%} item_clean

.PHONY: default_action item_setup item_build item_check all $(SUBTARGETS) $(SUBTARGETS_CLEAN) clean clean_dist
.SUFFIXES:

