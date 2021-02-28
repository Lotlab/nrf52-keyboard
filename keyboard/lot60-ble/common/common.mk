ROOT_DIR := $(COMMON_ROOT)/../../..
APP_MAIN_DIR := $(ROOT_DIR)/application/main
TMK_CORE_DIR := $(ROOT_DIR)/tmk/tmk_core
USB_SOURCE_DIR := $(ROOT_DIR)/usb

APP_PROJ_DIR := $(APP_MAIN_DIR)/project
APP_SRC_DIR := $(APP_MAIN_DIR)/src

# 配置项：主控芯片类型
# 可选：nrf52810 或 nrf52832
NRF_CHIP := nrf52810

CONFIG_H = config.h
CONFIG_H_DIR = $(COMMON_ROOT)
INC_FOLDERS += $(COMMON_ROOT)
SDCC_CFLAGS += -I $(COMMON_ROOT)
SRC_FILES += $(COMMON_ROOT)/keymap_plain.c

# 配置项：功能选项
BOOTMAGIC_ENABLE = no	# 启用Bootmagic
BOOTCHECK_ENABLE = yes  # 启用Bootchek
EXTRAKEY_ENABLE = yes	# 启用媒体键功能
NKRO_ENABLE = yes	# 启用USB的全键无冲功能
USB_6KRO_ENABLE = yes # 启用USB的六键无冲功能

# 如果不启用3灯LED状态，则启用内置的 RGB LED 指示
ifneq (yes,$(strip $(THREE_LED_STATUS)))
	SRC_FILES += $(COMMON_ROOT)/user_evt.c
	ONE_RGB_LED = yes
endif

# 控制各个硬件版本的差异
ifeq (REV_C,$(strip $(HARDWARE_VERSION)))
	OPT_DEFS += -DHARDWARE_REV_C
	SDCC_CFLAGS += -DHARDWARE_REV_C
else 
ifeq (REV_E,$(strip $(HARDWARE_VERSION)))
	OPT_DEFS += -DHARDWARE_REV_E
	SDCC_CFLAGS += -DHARDWARE_REV_E
	# 启用RESET PIN
	CONFIG_GPIO_AS_PINRESET = yes
else
ifeq (REV_F,$(strip $(HARDWARE_VERSION)))
	OPT_DEFS += -DHARDWARE_REV_F
	SDCC_CFLAGS += -DHARDWARE_REV_F
	# 启用RESET PIN
	CONFIG_GPIO_AS_PINRESET = yes
else
ifeq (REV_G,$(strip $(HARDWARE_VERSION)))
	OPT_DEFS += -DHARDWARE_REV_G
	SDCC_CFLAGS += -DHARDWARE_REV_G
	# 启用RESET PIN
	CONFIG_GPIO_AS_PINRESET = yes
	ONBOARD_CMSIS_DAP = yes
else
ifeq (REV_H,$(strip $(HARDWARE_VERSION)))
	OPT_DEFS += -DHARDWARE_REV_H
	SDCC_CFLAGS += -DHARDWARE_REV_H
	# 启用RESET PIN
	CONFIG_GPIO_AS_PINRESET = yes
	ONBOARD_CMSIS_DAP = yes
else
	$(error PLEASE SPECIFY HARDWARE_VERSION, AS REV_C, REV_E OR REV_F)
endif
endif
endif
endif
endif

all: default ch554

include $(APP_PROJ_DIR)/kbd.mk
include $(USB_SOURCE_DIR)/usb.mk