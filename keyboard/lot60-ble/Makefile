ROOT_DIR := ../..
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

CONFIG_H = config.h
INC_FOLDERS += ./
SRC_FILES += keymap_plain.c user_evt.c

# Build Options
#   comment out to disable the options.
#
BOOTMAGIC_ENABLE = yes	# Virtual DIP switch configuration(+1000)
EXTRAKEY_ENABLE = yes	# Audio control and System control(+450)
NKRO_ENABLE = yes	# USB Nkey Rollover
USB_6KRO_ENABLE = yes # USB 6Key Rollover


include $(APP_PROJ_DIR)/kbd.mk