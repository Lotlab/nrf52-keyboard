TMK_CORE_DIR = ../../../tmk/tmk_core
PROJ_DIR := ../src

# project specific files
SRC_FILES += $(PROJ_DIR)/keyboard/keyboard_matrix.c \
    $(PROJ_DIR)/keyboard/host_driver.c \
    $(PROJ_DIR)/keyboard/eeconfig.c \
    $(PROJ_DIR)/keyboard/ble_keyboard.c \
    $(PROJ_DIR)/keyboard/keyboard_timer.c \
    $(PROJ_DIR)/keyboard/glue_layer.c \
    $(PROJ_DIR)/keyboard/keyboard_fn.c \
    $(PROJ_DIR)/keyboard/keymap_plain.c \
    $(PROJ_DIR)/keyboard/keyboard_led.c \
    $(PROJ_DIR)/keyboard/passkey.c \
    $(PROJ_DIR)/keyboard/keyboard_bootmagic.c \
    $(PROJ_DIR)/keyboard/keymap_storage.c \
    $(PROJ_DIR)/keyboard/sleep_reason.c \
    $(PROJ_DIR)/keyboard/keyboard_evt.c \
    $(PROJ_DIR)/keyboard/usb_comm.c \

CONFIG_H = $(PROJ_DIR)/config/config.h

INC_FOLDERS += $(PROJ_DIR)/keyboard 

# Build Options
#   comment out to disable the options.
#
BOOTMAGIC_ENABLE = yes	# Virtual DIP switch configuration(+1000)
EXTRAKEY_ENABLE = yes	# Audio control and System control(+450)
# NKRO_ENABLE = yes	# USB Nkey Rollover [Not impliment yet]
USB_6KRO_ENABLE = yes # USB 6Key Rollover

# Version string
VERSION := $(shell (git describe --always --dirty || echo 'unknown') 2> /dev/null)
OPT_DEFS += -DVERSION=$(VERSION)

include tmk.mk
include app.mk