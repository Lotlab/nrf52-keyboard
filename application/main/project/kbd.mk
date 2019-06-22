ifndef TMK_CORE_DIR
TMK_CORE_DIR = ../../../tmk/tmk_core
endif

ifndef APP_SRC_DIR
APP_SRC_DIR := ../src
endif

# project specific files
SRC_FILES += $(APP_SRC_DIR)/keyboard/keyboard_matrix.c \
    $(APP_SRC_DIR)/keyboard/host_driver.c \
    $(APP_SRC_DIR)/keyboard/eeconfig.c \
    $(APP_SRC_DIR)/keyboard/ble_keyboard.c \
    $(APP_SRC_DIR)/keyboard/keyboard_timer.c \
    $(APP_SRC_DIR)/keyboard/glue_layer.c \
    $(APP_SRC_DIR)/keyboard/keyboard_fn.c \
    $(APP_SRC_DIR)/keyboard/keyboard_led.c \
    $(APP_SRC_DIR)/keyboard/passkey.c \
    $(APP_SRC_DIR)/keyboard/keyboard_bootmagic.c \
    $(APP_SRC_DIR)/keyboard/keymap_storage.c \
    $(APP_SRC_DIR)/keyboard/sleep_reason.c \
    $(APP_SRC_DIR)/keyboard/keyboard_evt.c \
    $(APP_SRC_DIR)/keyboard/usb_comm.c \

INC_FOLDERS += $(APP_SRC_DIR)/keyboard 

# Version string
VERSION := $(shell (git describe --always --dirty || echo 'unknown') 2> /dev/null)
OPT_DEFS += -DVERSION=$(VERSION)

include $(APP_PROJ_DIR)/tmk.mk
ifeq ($(MCU),nrf52832)
    include $(APP_PROJ_DIR)/app_nrf52832.mk
else
    include $(APP_PROJ_DIR)/app_nrf52810.mk
endif