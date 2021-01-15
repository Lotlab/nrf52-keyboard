ROOT_DIR := $(COMMON_ROOT)/../../..
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

# 特殊目录控制
# SDK_ROOT := /var/nrf52/nRF52_SDK
# TEMPLATE_PATH := /var/nrf52/template

# 配置项：主控芯片类型
# 可选：nrf52810 或 nrf52832
NRF_CHIP := nrf52832

# 配置文件文件名
CONFIG_H = config.h
CONFIG_H_DIR = $(COMMON_ROOT)
INC_FOLDERS += $(COMMON_ROOT)
SDCC_CFLAGS += -I $(COMMON_ROOT)

# 配置项：功能选项
# 关于更多的可配置项目，请参考doc目录下的相应文档
BOOTMAGIC_ENABLE = no	# 启用Bootmagic
BOOTCHECK_ENABLE = yes  # 启用Bootchek
EXTRAKEY_ENABLE = yes	# 启用媒体键功能
NKRO_ENABLE = yes	# 启用USB的全键无冲功能
USB_6KRO_ENABLE = yes # 启用USB的六键无冲功能
NRF52_DISABLE_FPU = yes # 禁用FPU

SSD1306_OLED = yes # 启用OLED
ROTARY_ENCODER = yes # 启用编码器
ONBOARD_CMSIS_DAP = yes # 启用板载调试器
# DEBUG = yes # 启用调试构建

ifeq (REV_0,$(strip $(HARDWARE_VERSION)))
	OPT_DEFS += -DHARDWARE_REV_0
	SDCC_CFLAGS += -DHARDWARE_REV_0
endif
ifeq (REV_1,$(strip $(HARDWARE_VERSION)))
	OPT_DEFS += -DHARDWARE_REV_1
	SDCC_CFLAGS += -DHARDWARE_REV_1
endif 

########## STOP HERE ##########
# 除非你知道下面这些配置项是干什么的，否则不要去动它

INC_FOLDERS += .

APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

all: default ch554

include $(APP_PROJ_DIR)/kbd.mk
include $(USB_SOURCE_DIR)/usb.mk