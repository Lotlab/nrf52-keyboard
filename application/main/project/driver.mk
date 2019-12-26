DRIVER_DIR := $(APP_SRC_DIR)/driver

INC_FOLDERS += $(DRIVER_DIR)

ifeq (yes,$(strip $(THREE_LED_STATUS)))
    SRC_FILES += $(DRIVER_DIR)/3led/3led_status.c
endif

ifeq (yes,$(strip $(ONE_RGB_LED)))
    SRC_FILES += $(DRIVER_DIR)/simple_rgb/led_rgb.c
endif
