set(DRIVER_DIR ${SRC_ROOT}/driver)

if(THREE_LED_STATUS)
    target_sources(${TARGET}.elf PRIVATE ${DRIVER_DIR}/3led/3led_status.c)
    target_compile_definitions(${TARGET}.elf PUBLIC THREE_LED_STATUS)
    target_compile_definitions(${TARGET}.elf PUBLIC THREE_LED_STATUS)
endif()

if(LEDMAP)
    target_sources(${TARGET}.elf PRIVATE ${DRIVER_DIR}/ledmap/ledmap.c)
    target_compile_definitions(${TARGET}.elf PUBLIC LEDMAP)
endif()

if(ONE_RGB_LED)
    target_sources(${TARGET}.elf PRIVATE ${DRIVER_DIR}/simple_rgb/led_rgb.c)
    target_compile_definitions(${TARGET}.elf PUBLIC ONE_RGB_LED)
endif()

if(SSD1306_OLED)
    set(I2C_DRIVER_ENABLE yes)
    target_sources(${TARGET}.elf PRIVATE 
        ${DRIVER_DIR}/ssd1306/ssd1306_oled.c
        ${DRIVER_DIR}/ssd1306/oled_graph.c
    )
    target_compile_definitions(${TARGET}.elf PUBLIC 
        SSD1306_OLED
    )
endif()

if(ROTARY_ENCODER)
    target_sources(${TARGET}.elf PRIVATE ${DRIVER_DIR}/rotary_encoder/encoder.c)
    target_compile_definitions(${TARGET}.elf PUBLIC 
        MATRIX_FORIGN_KEY
        ROTARY_ENCODER
    )
endif()

if(SHIFT_REGISTER_595)
    target_sources(${TARGET}.elf PRIVATE ${DRIVER_DIR}/shift_register/74hc595.c)
    target_compile_definitions(${TARGET}.elf PUBLIC 
        HC959_ENABLE
    )
endif()

if(RGBLIGHT_ENABLE)
    target_sources(${TARGET}.elf PRIVATE 
	    ${DRIVER_DIR}/rgb_light/rgblight.c
	    ${DRIVER_DIR}/rgb_light/rgblight_ctrl.c
    )
    target_compile_definitions(${TARGET}.elf PUBLIC 
        RGBLIGHT_ENABLE
    )
    set(RGBCOMMON_REQUIRED yes)
endif()

if(WS2812_ENABLE)
    target_sources(${TARGET}.elf PRIVATE
        ${DRIVER_DIR}/ws2812/ws2812.c
    )
    if(WS2812_AS_RGBLIGHT)
        target_sources(${TARGET}.elf PRIVATE 
            ${DRIVER_DIR}/ws2812/rgblight_ws2812.c
        )
    endif()
    if(WS2812_AS_RGBMATRIX)
        target_sources(${TARGET}.elf PRIVATE 
            ${DRIVER_DIR}/ws2812/rgbmatrix_ws2812.c
        )
    endif()
endif()

if(RGBMATRIX_ENABLE)
    target_sources(${TARGET}.elf PRIVATE 
	    ${DRIVER_DIR}/rgb_matrix/rgb_matrix.c
	    ${DRIVER_DIR}/rgb_matrix/rgb_matrix_ctrl.c
    )
    target_compile_definitions(${TARGET}.elf PUBLIC 
        RGBMATRIX_ENABLE
    )
    set(RGBCOMMON_REQUIRED yes)
endif()

if(RGBCOMMON_REQUIRED)
    target_sources(${TARGET}.elf PRIVATE 
        ${DRIVER_DIR}/rgb_common/led_tables.c
        ${DRIVER_DIR}/rgb_common/color.c
    )
    target_link_libraries(${TARGET}.elf m)
endif()

if(AW9110_ENABLE)
    target_sources(${TARGET}.elf PRIVATE 
        ${DRIVER_DIR}/aw9110/aw9110.c
    )
    target_compile_definitions(${TARGET}.elf PUBLIC 
        AW9110_ENABLE
    )
    set(I2C_DRIVER_ENABLE yes)
endif()

if(I2C_DRIVER_ENABLE)
    target_sources(${TARGET}.elf PRIVATE ${DRIVER_DIR}/i2c/i2c.c)
    target_compile_definitions(${TARGET}.elf PUBLIC I2C_DRIVER_ENABLE)
endif()

if(SHARED_I2C_DRIVER)
    target_sources(${TARGET}.elf PRIVATE ${DRIVER_DIR}/i2c/shared_i2c.c)
    target_compile_definitions(${TARGET}.elf PUBLIC SHARED_I2C_DRIVER)
endif()
