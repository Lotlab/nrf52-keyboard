
# SET(NRF52_SDK_ROOT "nrf52/sdk")
SET(NRF52_SDK_ROOT "../SDK")

if(NOT DEFINED TARGET_CHIP)
    message("Please set TARGET_CHIP in your device specific cmake")
endif()

string(TOLOWER ${TARGET_CHIP} TARGET_CHIP_LOWER)
include(${CMAKE_CURRENT_LIST_DIR}/nrf52/${TARGET_CHIP_LOWER}.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/nrf52/arm-none-eabi.cmake)

if(CONFIG_GPIO_AS_PINRESET)
    target_compile_definitions(${TARGET_EXEC} PRIVATE "CONFIG_GPIO_AS_PINRESET")
endif()

target_sources(${TARGET_EXEC} PRIVATE 
    ${NRF52_SDK_ROOT}/components/libraries/util/app_error.c
    ${NRF52_SDK_ROOT}/components/libraries/util/app_error_handler_gcc.c
    ${NRF52_SDK_ROOT}/components/libraries/util/app_error_weak.c
    ${NRF52_SDK_ROOT}/components/libraries/timer/app_timer.c 
    ${NRF52_SDK_ROOT}/components/libraries/scheduler/app_scheduler.c
    ${NRF52_SDK_ROOT}/components/libraries/util/app_util_platform.c
    ${NRF52_SDK_ROOT}/components/libraries/crc16/crc16.c
    ${NRF52_SDK_ROOT}/components/libraries/fds/fds.c
    ${NRF52_SDK_ROOT}/components/libraries/hardfault/hardfault_implementation.c
    ${NRF52_SDK_ROOT}/components/libraries/util/nrf_assert.c
    ${NRF52_SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c
    ${NRF52_SDK_ROOT}/components/libraries/atomic_flags/nrf_atflags.c
    ${NRF52_SDK_ROOT}/components/libraries/atomic/nrf_atomic.c
    ${NRF52_SDK_ROOT}/components/libraries/balloc/nrf_balloc.c
    ${NRF52_SDK_ROOT}/external/fprintf/nrf_fprintf.c
    ${NRF52_SDK_ROOT}/external/fprintf/nrf_fprintf_format.c
    ${NRF52_SDK_ROOT}/components/libraries/fstorage/nrf_fstorage.c
    ${NRF52_SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_sd.c
    ${NRF52_SDK_ROOT}/components/libraries/memobj/nrf_memobj.c
    ${NRF52_SDK_ROOT}/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
    ${NRF52_SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c
    ${NRF52_SDK_ROOT}/components/libraries/experimental_section_vars/nrf_section_iter.c
    ${NRF52_SDK_ROOT}/components/libraries/strerror/nrf_strerror.c
    ${NRF52_SDK_ROOT}/components/libraries/bootloader/dfu/nrf_dfu_svci.c
    ${NRF52_SDK_ROOT}/components/libraries/low_power_pwm/low_power_pwm.c
    ${NRF52_SDK_ROOT}/components/libraries/uart/app_uart_fifo.c
    ${NRF52_SDK_ROOT}/components/libraries/fifo/app_fifo.c
    ${NRF52_SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c
    ${NRF52_SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c
    ${NRF52_SDK_ROOT}/modules/nrfx/drivers/src/nrfx_clock.c
    ${NRF52_SDK_ROOT}/modules/nrfx/drivers/src/prs/nrfx_prs.c
    ${NRF52_SDK_ROOT}/modules/nrfx/drivers/src/nrfx_saadc.c
    ${NRF52_SDK_ROOT}/modules/nrfx/drivers/src/nrfx_wdt.c
    ${NRF52_SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c
    ${NRF52_SDK_ROOT}/external/segger_rtt/SEGGER_RTT.c
    ${NRF52_SDK_ROOT}/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
    ${NRF52_SDK_ROOT}/external/segger_rtt/SEGGER_RTT_printf.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/auth_status_tracker.c
    ${NRF52_SDK_ROOT}/components/ble/common/ble_advdata.c
    ${NRF52_SDK_ROOT}/components/ble/ble_advertising/ble_advertising.c
    ${NRF52_SDK_ROOT}/components/ble/common/ble_conn_params.c
    ${NRF52_SDK_ROOT}/components/ble/common/ble_conn_state.c
    ${NRF52_SDK_ROOT}/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c
    ${NRF52_SDK_ROOT}/components/ble/common/ble_srv_common.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/gatt_cache_manager.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/gatts_cache_manager.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/id_manager.c
    ${NRF52_SDK_ROOT}/components/ble/nrf_ble_gatt/nrf_ble_gatt.c
    ${NRF52_SDK_ROOT}/components/ble/nrf_ble_qwr/nrf_ble_qwr.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/peer_data_storage.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/peer_database.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/peer_id.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/peer_manager.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/peer_manager_handler.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/pm_buffer.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/security_dispatcher.c
    ${NRF52_SDK_ROOT}/components/ble/peer_manager/security_manager.c
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_bas/ble_bas.c
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_dis/ble_dis.c
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_hids/ble_hids.c
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_dfu/ble_dfu.c
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_dfu/ble_dfu_bonded.c
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_dfu/ble_dfu_unbonded.c
    ${NRF52_SDK_ROOT}/components/softdevice/common/nrf_sdh.c
    ${NRF52_SDK_ROOT}/components/softdevice/common/nrf_sdh_ble.c
    ${NRF52_SDK_ROOT}/components/softdevice/common/nrf_sdh_soc.c
)

target_include_directories(${TARGET_EXEC} PRIVATE
    ${NRF52_SDK_ROOT}/components/libraries/pwm
    ${NRF52_SDK_ROOT}/modules/nrfx/hal
    ${NRF52_SDK_ROOT}/components/libraries/log
    ${NRF52_SDK_ROOT}/components/libraries/fstorage
    ${NRF52_SDK_ROOT}/components/libraries/mutex
    ${NRF52_SDK_ROOT}/components/libraries/gpiote
    ${NRF52_SDK_ROOT}/components/libraries/bootloader/ble_dfu
    ${NRF52_SDK_ROOT}/components/ble/ble_advertising
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_bas_c
    ${NRF52_SDK_ROOT}/modules/nrfx/drivers/include
    ${NRF52_SDK_ROOT}/components/libraries/experimental_task_manager
    ${NRF52_SDK_ROOT}/components/libraries/queue
    ${NRF52_SDK_ROOT}/components/libraries/pwr_mgmt
    ${NRF52_SDK_ROOT}/components/toolchain/cmsis/include
    ${NRF52_SDK_ROOT}/components/ble/common
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_bas
    ${NRF52_SDK_ROOT}/components/libraries/mpu
    ${NRF52_SDK_ROOT}/components/libraries/experimental_section_vars
    ${NRF52_SDK_ROOT}/components/libraries/slip
    ${NRF52_SDK_ROOT}/components/libraries/delay
    ${NRF52_SDK_ROOT}/components/libraries/csense_drv
    ${NRF52_SDK_ROOT}/components/libraries/memobj
    ${NRF52_SDK_ROOT}/components/libraries/bootloader
    ${NRF52_SDK_ROOT}/components/libraries/bootloader/dfu
    ${NRF52_SDK_ROOT}/components/libraries/bootloader/ble_dfu
    ${NRF52_SDK_ROOT}/components/libraries/low_power_pwm
    ${NRF52_SDK_ROOT}/components/softdevice/common
    ${NRF52_SDK_ROOT}/components/libraries/low_power_pwm
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_dfu
    ${NRF52_SDK_ROOT}/external/fprintf
    ${NRF52_SDK_ROOT}/components/libraries/svc
    ${NRF52_SDK_ROOT}/components/libraries/atomic
    ${NRF52_SDK_ROOT}/components
    ${NRF52_SDK_ROOT}/components/libraries/scheduler
    ${NRF52_SDK_ROOT}/components/libraries/cli
    ${NRF52_SDK_ROOT}/components/libraries/crc16
    ${NRF52_SDK_ROOT}/components/libraries/util
    ${NRF52_SDK_ROOT}/components/libraries/csense
    ${NRF52_SDK_ROOT}/components/libraries/balloc
    ${NRF52_SDK_ROOT}/components/libraries/ecc
    ${NRF52_SDK_ROOT}/components/libraries/hardfault
    ${NRF52_SDK_ROOT}/components/libraries/hci
    ${NRF52_SDK_ROOT}/components/libraries/timer
    ${NRF52_SDK_ROOT}/components/libraries/timer/experimental
    ${NRF52_SDK_ROOT}/components/libraries/uart
    ${NRF52_SDK_ROOT}/components/libraries/fifo
    ${NRF52_SDK_ROOT}/integration/nrfx
    ${NRF52_SDK_ROOT}/components/libraries/sortlist
    ${NRF52_SDK_ROOT}/components/libraries/spi_mngr
    ${NRF52_SDK_ROOT}/components/libraries/led_softblink
    ${NRF52_SDK_ROOT}/modules/nrfx/mdk
    ${NRF52_SDK_ROOT}/components/ble/ble_link_ctx_manager
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_hids
    ${NRF52_SDK_ROOT}/components/libraries/strerror
    ${NRF52_SDK_ROOT}/components/libraries/crc32
    ${NRF52_SDK_ROOT}/components/ble/peer_manager
    ${NRF52_SDK_ROOT}/components/libraries/mem_manager
    ${NRF52_SDK_ROOT}/components/libraries/ringbuf
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_tps
    ${NRF52_SDK_ROOT}/components/ble/ble_services/ble_dis
    ${NRF52_SDK_ROOT}/components/ble/nrf_ble_gatt
    ${NRF52_SDK_ROOT}/components/ble/nrf_ble_qwr
    ${NRF52_SDK_ROOT}/modules/nrfx
    ${NRF52_SDK_ROOT}/integration/nrfx/legacy
    ${NRF52_SDK_ROOT}/external/segger_rtt
    ${NRF52_SDK_ROOT}/components/libraries/atomic_fifo
    ${NRF52_SDK_ROOT}/components/libraries/crypto
    ${NRF52_SDK_ROOT}/components/ble/ble_racp
    ${NRF52_SDK_ROOT}/components/libraries/fds
    ${NRF52_SDK_ROOT}/components/libraries/atomic_flags
    ${NRF52_SDK_ROOT}/components/libraries/stack_guard
    ${NRF52_SDK_ROOT}/components/libraries/log/src
)

target_include_directories(tmk PRIVATE 
    ${NRF52_SDK_ROOT}/components/libraries/delay
    ${NRF52_SDK_ROOT}/components/toolchain/cmsis/include
    ${NRF52_SDK_ROOT}/components/libraries/util
    ${NRF52_SDK_ROOT}/modules/nrfx
    ${NRF52_SDK_ROOT}/modules/nrfx/mdk
    ${NRF52_SDK_ROOT}/integration/nrfx
    ${SRC_ROOT}/config
)

target_compile_definitions(${TARGET_EXEC} PRIVATE 
    "NRF52_APP"
    "NRF52_PAN_74"
    "NRFX_COREDEP_DELAY_US_LOOP_CYCLES=3"
    "NRF_DFU_SVCI_ENABLED"
    "NRF_DFU_TRANSPORT_BLE=1"
    "NRF_SD_BLE_API_VERSION=6"
    "SOFTDEVICE_PRESENT"
    "SWI_DISABLE0"
    "__HEAP_SIZE=${NRF52_HEAP_SIZE}"
    "__STACK_SIZE=${NRF52_STACK_SIZE}"
)

target_compile_options(${TARGET_EXEC} PRIVATE
    "-fno-builtin" # 无Buildin函数，是否可以作为全局选项？
    "-mcpu=cortex-m4"
    "-mthumb"
    "-mabi=aapcs"
)

if(NRF52_SOFTDEVICE_TYPE STREQUAL "S112")
    target_compile_definitions(${TARGET_EXEC} PRIVATE "S112")
    SET(NRF52_SOFTDEVICE_NAME "s112_nrf52_6.1.1_softdevice.hex")
    SET(SOFTDEVICE_VER "0xb8")
    SET(SOFTDEVICE_PATH ${NRF52_SDK_ROOT}/components/softdevice/s112/hex/${NRF52_SOFTDEVICE_NAME})
    target_include_directories(${TARGET_EXEC} PRIVATE
        ${NRF52_SDK_ROOT}/components/softdevice/s112/headers/nrf52
        ${NRF52_SDK_ROOT}/components/softdevice/s112/headers
    )
    target_include_directories(tmk PRIVATE
        ${NRF52_SDK_ROOT}/components/softdevice/s112/headers/nrf52
        ${NRF52_SDK_ROOT}/components/softdevice/s112/headers
    )
elseif(NRF52_SOFTDEVICE_TYPE STREQUAL "S132")
    target_compile_definitions(${TARGET_EXEC} PRIVATE "S132")
    SET(NRF52_SOFTDEVICE_NAME "s132_nrf52_6.1.1_softdevice.hex")
    SET(SOFTDEVICE_VER "0xb7")
    SET(SOFTDEVICE_PATH ${NRF52_SDK_ROOT}/components/softdevice/s132/hex/${NRF52_SOFTDEVICE_NAME})
    target_include_directories(${TARGET_EXEC} PRIVATE
        ${NRF52_SDK_ROOT}/components/softdevice/s132/headers/nrf52
        ${NRF52_SDK_ROOT}/components/softdevice/s132/headers
    )
    target_include_directories(tmk PRIVATE
        ${NRF52_SDK_ROOT}/components/softdevice/s132/headers/nrf52
        ${NRF52_SDK_ROOT}/components/softdevice/s132/headers
    )
else()
    message("INTERNAL ERROR: SOFTDEVICE TYPE IS NOT SET.")
endif()

get_filename_component(LINK_SCRIPT_DIR "${NRF52_SDK_ROOT}/modules/nrfx/mdk" REALPATH)
get_filename_component(SIGN_KEY "${CMAKE_CURRENT_LIST_DIR}/nrf52/private.key" REALPATH)
get_filename_component(SOFTDEVICE_REAL_PATH ${SOFTDEVICE_PATH} REALPATH)

target_link_options(${TARGET_EXEC} PRIVATE
    "-L${LINK_SCRIPT_DIR}"
    # "-lnosys"
    "-mthumb"
    "-mabi=aapcs"
)

# Post processing command to create a hex file 
add_custom_command(TARGET ${TARGET_EXEC} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${TARGET_EXEC} ${TARGET}.hex
    COMMENT "Invoking: Hexdump elf")

add_custom_command(TARGET ${TARGET_EXEC}  POST_BUILD
    COMMAND nrfutil settings generate --family ${NRF52_FLASH_FAMILY} --application ${TARGET}.hex --application-version 1 --bootloader-version 1 --bl-settings-version 1 ${TARGET}_sign.hex
    COMMENT "Invoking: Generate DFU Sign")

add_custom_command(TARGET ${TARGET_EXEC}  POST_BUILD
    COMMAND nrfutil pkg generate 
    --hw-version 52 
    --application-version 1 
    --application ${TARGET}.hex
    --sd-req ${SOFTDEVICE_VER}
    --key-file ${SIGN_KEY} 
    ${TARGET}.zip
    COMMENT "Invoking: Generate Package")

add_custom_command(TARGET ${TARGET_EXEC} POST_BUILD
    COMMAND mergehex -m
    ${SOFTDEVICE_REAL_PATH}
    ${TARGET}_sign.hex
    ${TARGET}.hex
    -o ${TARGET}_all.hex
    COMMENT "Invoking: Merge ALL."
)


