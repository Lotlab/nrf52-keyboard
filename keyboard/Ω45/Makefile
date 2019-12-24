ROOT_DIR := ../..
USER_SRC_DIR := ./user_src
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

# 配置项：主控芯片类型
# 可选：nrf52810 或 nrf52832
NRF_CHIP := nrf52832

CONFIG_H = config.h
INC_FOLDERS += \
    . \
	$(USER_SRC_DIR) \
	$(APP_SRC_DIR)/ble

SRC_FILES += \
	$(USER_SRC_DIR)/keymap_plain.c \
	$(USER_SRC_DIR)/user_evt.c \
	$(USER_SRC_DIR)/status_led.c \
	$(USER_SRC_DIR)/user_fn.c \
	$(USER_SRC_DIR)/user_func.c \
	$(USER_SRC_DIR)/user_bootmagic.c

# 配置项：功能选项
BOOTMAGIC_ENABLE = yes	# 启用Bootmagic
EXTRAKEY_ENABLE = yes	# 启用媒体键功能
NKRO_ENABLE = yes	# 启用USB的全键无冲功能
USB_6KRO_ENABLE = yes # 启用USB的六键无冲功能
COMMAND_ENABLE = yes    # 启用调试和配置的命令

all: default ch554

include $(APP_PROJ_DIR)/kbd.mk
include $(USB_SOURCE_DIR)/usb.mk
