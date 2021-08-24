DRIVER_DIR := $(APP_SRC_DIR)/driver

INC_FOLDERS += $(DRIVER_DIR)

ifeq (yes,$(strip $(THREE_LED_STATUS)))
    SRC_FILES += $(DRIVER_DIR)/3led/3led_status.c
    INC_FOLDERS += $(DRIVER_DIR)/3led
    OPT_DEFS += -DTHREE_LED_STATUS
endif

ifeq (yes,$(strip $(LEDMAP)))
    SRC_FILES += $(DRIVER_DIR)/ledmap/ledmap.c
    OPT_DEFS += -DLEDMAP
endif

ifeq (yes,$(strip $(ONE_RGB_LED)))
    SRC_FILES += $(DRIVER_DIR)/simple_rgb/led_rgb.c
    OPT_DEFS += -DONE_RGB_LED
endif

ifeq (yes,$(strip $(SSD1306_OLED)))
    SHARED_I2C_DRIVER := yes
    SRC_FILES += $(DRIVER_DIR)/ssd1306/ssd1306_oled.c \
                 $(DRIVER_DIR)/ssd1306/oled_graph.c \
                 $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c
    OPT_DEFS += -DTWI0_ENABLED=1 -DTWI_ENABLED=1 -DNRFX_TWI0_ENABLED=1 -DSSD1306_OLED
endif

ifeq (yes,$(strip $(ROTARY_ENCODER)))
    SRC_FILES += $(DRIVER_DIR)/rotary_encoder/encoder.c \
                 $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_qdec.c
    OPT_DEFS += -DNRFX_QDEC_ENABLED=1 -DQDEC_ENABLED=1 \
                -DMATRIX_FORIGN_KEY \
                -DROTARY_ENCODER
endif

ifeq (yes,$(strip $(RGBLIGHT_ENABLE)))
    INC_FOLDERS += \
	    $(DRIVER_DIR)/ws2812

    SRC_FILES +=$(DRIVER_DIR)/ws2812/ws2812.c \
	            $(DRIVER_DIR)/ws2812/rgblight.c \
	            $(DRIVER_DIR)/ws2812/led_tables.c \
	            $(DRIVER_DIR)/ws2812/rgblight_ctrl.c \
	            $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_pwm.c
    OPT_DEFS += -DNRFX_PWM_ENABLED=1 -DNRFX_PWM0_ENABLED=1 -DPWM_ENABLED=1 -DPWM0_ENABLED=1 -DRGBLIGHT_ENABLE
endif

ifeq (yes,$(strip $(SHARED_I2C_DRIVER)))
    INC_FOLDERS += $(DRIVER_DIR)/i2c
    SRC_FILES += $(DRIVER_DIR)/i2c/shared_i2c.c
    OPT_DEFS += -DSHARED_I2C_DRIVER
endif
