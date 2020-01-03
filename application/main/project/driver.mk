DRIVER_DIR := $(APP_SRC_DIR)/driver

INC_FOLDERS += $(DRIVER_DIR)

ifeq (yes,$(strip $(THREE_LED_STATUS)))
    SRC_FILES += $(DRIVER_DIR)/3led/3led_status.c
endif

ifeq (yes,$(strip $(ONE_RGB_LED)))
    SRC_FILES += $(DRIVER_DIR)/simple_rgb/led_rgb.c
endif

ifeq (yes,$(strip $(SSD1306_OLED)))
    SRC_FILES += $(DRIVER_DIR)/ssd1306/ssd1306_oled.c \
                 $(DRIVER_DIR)/ssd1306/oled_graph.c \
                 $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c
    OPT_DEFS += -DTWI0_ENABLED=1 -DTWI_ENABLED=1 -DNRFX_TWI0_ENABLED=1
endif

ifeq (yes,$(strip $(ROTARY_ENCODER)))
    SRC_FILES += $(DRIVER_DIR)/rotary_encoder/encoder.c \
                 $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_qdec.c
    OPT_DEFS += -DNRFX_QDEC_ENABLED=1 -DQDEC_ENABLED=1 \
                -DMATRIX_FORIGN_KEY
endif