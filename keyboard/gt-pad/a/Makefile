ROOT_DIR := ../../..
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src
USER_SRC_DIR := $(APP_SRC_DIR)/user/glab

CONFIG_H = config.h
CONFIG_H_DIR = .

include ./rules.mk

INC_FOLDERS += \
	. \
	$(USER_SRC_DIR) \


ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRC_FILES += \
	actionmap_plain.c \
	$(USER_SRC_DIR)/user_fn.c
else
	SRC_FILES += \
	keymap_plain.c \
	$(USER_SRC_DIR)/user_fn.c
endif


all: default ch554

include $(APP_PROJ_DIR)/kbd.mk
include $(USB_SOURCE_DIR)/usb.mk
