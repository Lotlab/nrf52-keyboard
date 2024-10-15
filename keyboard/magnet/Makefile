# 此工程的根目录
ROOT_DIR := ../..

# 特殊目录控制
SDK_ROOT := $(ROOT_DIR)/SDK
TEMPLATE_PATH := $(ROOT_DIR)/template
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb
APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src
INC_FOLDERS += .

# 配置文件文件名
CONFIG_H = config.h
CONFIG_H_DIR = .
SRC_FILES += matrix_analog.c keymap_plain.c

include ./rules.mk

purge:
	-rm -rf $(OUTPUT_DIR)

go: purge ch554 package merge_bootloader_all

all: default ch554

include $(APP_PROJ_DIR)/kbd.mk
include $(USB_SOURCE_DIR)/usb.mk
