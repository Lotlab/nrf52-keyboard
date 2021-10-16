target_sources(${TARGET_EXEC} PRIVATE
    ${NRF52_SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52810.S
    ${NRF52_SDK_ROOT}/modules/nrfx/mdk/system_nrf52810.c
)

get_filename_component(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/nrf52810_s112.ld" REALPATH)

target_compile_definitions(${TARGET_EXEC} PRIVATE
    "FLOAT_ABI_SOFT"
    "NRF52810_XXAA"
)
target_compile_definitions(tmk PRIVATE
    "NRF52"
    "NRF52810_XXAA"
)
target_compile_options(${TARGET_EXEC} PRIVATE
    "-mfloat-abi=soft"
)
target_compile_options(tmk PRIVATE
    "-mfloat-abi=soft"
)

target_link_options(${TARGET_EXEC} PRIVATE
    "-T" "${LINKER_SCRIPT}"
)

SET(NRF52_SOFTDEVICE_TYPE "S112")
SET(NRF52_HEAP_SIZE 2048)
SET(NRF52_STACK_SIZE 2048)
SET(NRF52_FLASH_FAMILY "NRF52810")
