ROOT_DIR := ../..
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

# 配置项：主控芯片类型
# 可选：nrf52810 或 nrf52832
NRF_CHIP := nrf52810

CONFIG_H = config.h
INC_FOLDERS += .
SRC_FILES += keymap_plain.c

# 配置项：功能选项
BOOTMAGIC_ENABLE = yes	# 启用Bootmagic
EXTRAKEY_ENABLE = yes	# 启用媒体键功能

# 注意：NKRO和6KRO存在冲突，不可以同时启用
NKRO_ENABLE = yes	# 启用USB的全键无冲功能
# USB_6KRO_ENABLE = yes # 启用USB的六键无冲功能

# 如果不启用3灯LED状态，则启用内置的 RGB LED 指示
ifneq (yes,$(strip $(THREE_LED_STATUS_EVT)))
	SRC_FILES += user_evt.c
	ONE_RGB_LED = yes
endif

all: default ch554

include $(APP_PROJ_DIR)/kbd.mk
include $(USB_SOURCE_DIR)/usb.mk