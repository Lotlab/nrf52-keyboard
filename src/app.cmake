
target_sources(${TARGET_EXEC} PRIVATE
    ${SRC_ROOT}/main.c
    ${SRC_ROOT}/ble/ble_services.c
    ${SRC_ROOT}/ble/ble_hid_service.c
    ${SRC_ROOT}/ble/ble_bas_service.c

    ${SRC_ROOT}/keyboard/keyboard_matrix.c
    ${SRC_ROOT}/keyboard/host_driver.c
    ${SRC_ROOT}/keyboard/ble_keyboard.c
    ${SRC_ROOT}/tmk/keyboard_timer.c
    ${SRC_ROOT}/tmk/glue_layer.c
    ${SRC_ROOT}/keyboard/keyboard_fn.c
    ${SRC_ROOT}/keyboard/keyboard_led.c
    ${SRC_ROOT}/keyboard/passkey.c
    ${SRC_ROOT}/keyboard/data_storage.c
    ${SRC_ROOT}/protocol/hid_configuration.c
    ${SRC_ROOT}/keyboard/sleep_reason.c
    ${SRC_ROOT}/keyboard/keyboard_evt.c
    ${SRC_ROOT}/keyboard/keyboard_battery.c
    ${SRC_ROOT}/keyboard/store_config.c
    ${SRC_ROOT}/protocol/usb_comm.c
    ${SRC_ROOT}/protocol/ble_comm.c
    ${SRC_ROOT}/keyboard/macro_player.c
    ${SRC_ROOT}/keyboard/power_save.c
    ${SRC_ROOT}/keyboard/power_button.c
    ${SRC_ROOT}/keyboard/adc_convert.c
)

target_include_directories(${TARGET_EXEC} PRIVATE
    ${SRC_ROOT}
    ${SRC_ROOT}/config
    ${SRC_ROOT}/keyboard
    ${SRC_ROOT}/tmk
    ${SRC_ROOT}/protocol
    ${SRC_ROOT}/ble
    ${TMK_CORE_ROOT}/common
)
