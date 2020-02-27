ROOT_DIR := ../..
USER_SRC_DIR := ./user_src
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

CONFIG_H = config.h

include ./rules.mk

INC_FOLDERS += \
    . \

SRC_FILES += \
	keymap_plain.c \

all: default ch554

include $(APP_PROJ_DIR)/kbd.mk
include $(USB_SOURCE_DIR)/usb.mk
