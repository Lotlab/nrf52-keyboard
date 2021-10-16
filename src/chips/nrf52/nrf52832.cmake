
target_sources(${TARGET_EXEC} PRIVATE
    ${NRF52_SDK_ROOT}/modules/nrfx/mdk/system_nrf52.c
    ${NRF52_SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52.S
)

get_filename_component(LINKER_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/nrf52832_s132.ld" REALPATH)

target_compile_definitions(${TARGET_EXEC} PRIVATE
    "NRF52"
    "NRF52832_XXAA"
    "CONFIG_NFCT_PINS_AS_GPIOS"
)
target_compile_definitions(tmk PRIVATE
    "NRF52"
    "NRF52832_XXAA"
)

target_link_options(${TARGET_EXEC} PRIVATE
    "-T" "${LINKER_SCRIPT}"
)

# 暂时用不了，禁用
SET(NRF52_DISABLE_FPU yes)

if(NRF52_DISABLE_FPU)
target_compile_definitions(${TARGET_EXEC} PRIVATE
    "FLOAT_ABI_SOFT"
)
target_compile_options(${TARGET_EXEC} PRIVATE
    "-mfloat-abi=soft"
)
target_compile_options(tmk PRIVATE
    "-mfloat-abi=soft"
)
else()
target_compile_definitions(${TARGET_EXEC} PRIVATE
    "FLOAT_ABI_HARD"
)
target_compile_options(${TARGET_EXEC} PRIVATE
    "-mfloat-abi=hard"
    "-mfpu=fpv4-sp-d16"
)
target_compile_options(tmk PRIVATE
    "-mfloat-abi=hard"
    "-mfpu=fpv4-sp-d16"
)
target_link_options(${TARGET_EXEC} PRIVATE
    "-mfloat-abi=hard"
    "-mfpu=fpv4-sp-d16"
)
target_link_options(tmk PRIVATE
    "-mfloat-abi=hard"
    "-mfpu=fpv4-sp-d16"
)
endif()

SET(NRF52_SOFTDEVICE_TYPE "S132")
SET(NRF52_HEAP_SIZE 2048)
SET(NRF52_STACK_SIZE 2048)
SET(NRF52_FLASH_FAMILY "NRF52")
