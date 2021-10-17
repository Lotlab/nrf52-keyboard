cmake_minimum_required(VERSION 3.10)

include("${CMAKE_CURRENT_LIST_DIR}/sdcc.cmake")

project(LotKeyboardUSB LANGUAGES C)

set(SRC_ROOT ".")
set(TARGET usb)
set(TARGET_EXEC ${TARGET}) # 目标可执行文件名称

add_executable(${TARGET_EXEC})

if(DEFINED EXTERNAL_CMAKE_FILE)
    include(${EXTERNAL_CMAKE_FILE})
    message("Include extra cmake rules: ${EXTERNAL_CMAKE_FILE}")
endif()

# 添加额外的include
if(DEFINED EXTERNAL_INC_DIR)
    target_include_directories(${TARGET_EXEC} PRIVATE ${EXTERNAL_INC_DIR})
    message("Include extra dir: ${EXTERNAL_INC_DIR}")
endif()

target_sources(${TARGET_EXEC} PRIVATE 
    ${SRC_ROOT}/main.c
    ${SRC_ROOT}/endpoints.c
    ${SRC_ROOT}/system.c
    ${SRC_ROOT}/uart.c
    ${SRC_ROOT}/interrupt.c
    ${SRC_ROOT}/descriptor.c
)

# USB 功能配置区
if(ONBOARD_CMSIS_DAP)
target_sources(${TARGET_EXEC} PRIVATE 
    ${SRC_ROOT}/DAP.c
    ${SRC_ROOT}/DAP_hid.c
    ${SRC_ROOT}/SW_DP.c
)
target_compile_definitions(${TARGET_EXEC} PRIVATE
    "ONBOARD_CMSIS_DAP"
)
endif()

if(NKRO_ENABLE)
target_compile_definitions(${TARGET_EXEC} PRIVATE
    "NKRO_ENABLE"
)
endif()

if(MOUSEKEY_ENABLE)
target_compile_definitions(${TARGET_EXEC} PRIVATE
    "MOUSE_ENABLE"
)
endif()

target_compile_options(${TARGET_EXEC} PRIVATE
    "-mmcs51"
    "--model-small"
    "--stack-auto"
    "--std-c11"
    "--opt-code-size"
    "-I" "."
)

target_link_options(${TARGET_EXEC} PRIVATE
    "--xram-size" "1024"
    "--iram-size" "256"
    "--code-size" "14336"
    "--out-fmt-ihx"
    "--xram-loc" "0"
)

add_custom_command(TARGET ${TARGET_EXEC} POST_BUILD
    COMMAND packihx ${TARGET}.ihx > ${TARGET}.hex
    COMMENT "Invoking: generate hex."
)

add_custom_command(TARGET ${TARGET_EXEC} POST_BUILD
    COMMAND makebin -p ${TARGET}.ihx ${TARGET}.bin
    COMMENT "Invoking: generate bin."
)
