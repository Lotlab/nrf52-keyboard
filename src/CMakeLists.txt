cmake_minimum_required(VERSION 3.10)

set(PROJECT_ROOT "..") # 工程根目录
# set(SRC_ROOT ".") # 源代码根目录
set(SRC_ROOT "../application/main/src") # 源代码根目录
set(TMK_CORE_ROOT "../tmk/tmk_core") # TMK Core 目录
set(TARGET main) # 目标名称
set(TARGET_EXEC ${TARGET}.elf) # 目标可执行文件名称

add_library(tmk STATIC)
add_executable(${TARGET_EXEC})

if(DEFINED EXTERNAL_CMAKE_FILE)
    include(${EXTERNAL_CMAKE_FILE})
    message("Include extra cmake rules: ${EXTERNAL_CMAKE_FILE}")
endif()

if(NOT CONFIG_H)
    set(CONFIG_H config.h)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(DEBUG true)
    message("Project build in debug mode.")
endif()

SET(COMMON_COMPILE_OPTIONS 
    "-Os"
    "-g3"
    "-Wall"
    "-fshort-enums"
    "-ffunction-sections"
    "-fdata-sections"
    "-fno-strict-aliasing"
)

target_compile_options(${TARGET_EXEC} PRIVATE 
    ${COMMON_COMPILE_OPTIONS}
)
target_compile_options(tmk PRIVATE 
    ${COMMON_COMPILE_OPTIONS}
    "-include" "${CONFIG_H}"
)

include(chips.cmake)
project(LotKeyboard LANGUAGES C ASM)

include(app.cmake)

include(${SRC_ROOT}/driver/driver.cmake)
include(tmk.cmake)
include(version.cmake)

# 调试状态下设置特殊的版本号
if(DEBUG)
target_compile_definitions(${TARGET_EXEC} PRIVATE 
    DEBUG
    VERSION=0xDEB0DEB0
)
else()
target_compile_definitions(${TARGET_EXEC} PRIVATE 
    BUILD_TIME=${BUILD_TIME}
    VERSION=0x${GIT_HASH}
)
endif()

target_compile_definitions(${TARGET_EXEC} PRIVATE 
    USE_CUSTOM_CONFIG
)

# 添加额外的include
if(DEFINED EXTERNAL_INC_DIR)
    target_include_directories(${TARGET_EXEC} PRIVATE ${EXTERNAL_INC_DIR})
    target_include_directories(tmk PRIVATE ${EXTERNAL_INC_DIR})
    message("Include extra dir: ${EXTERNAL_INC_DIR}")
endif()

target_link_libraries(${TARGET}.elf tmk)

target_link_options(${TARGET_EXEC} PRIVATE 
    "--specs=nosys.specs" # newlib 标准库选项
    "--specs=nano.specs" # newlib 标准库选项
    # "-nostartfiles" # 不要启动文件
    "-fno-exceptions" # Without exception
    "-Wl,-Map=${TARGET}.map" # 打印.map文件
    "-Wl,--gc-sections" # 删除无用
    "-Wl,--print-memory-usage" # 输出用量
)

# add_custom_command(TARGET ${TARGET_EXEC} POST_BUILD
#         COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET_EXEC} ${TARGET}.bin
#         COMMENT "Invoking: Hexdump bin")
