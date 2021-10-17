SET(CHIP_FAMILY NRF52) # 芯片家族
SET(TARGET_CHIP NRF52832) # 芯片类型

set(BOOTMAGIC_ENABLE no)   # 禁用 Bootmagic
set(BOOTCHECK_ENABLE yes)  # 启用 Bootchek
set(EXTRAKEY_ENABLE yes)   # 启用媒体键功能
set(NKRO_ENABLE yes)       # 启用USB的全键无冲功能
set(USB_6KRO_ENABLE yes)   # 启用USB的六键无冲功能
set(MOUSEKEY_ENABLE yes)   # 启用鼠标键
set(COMMAND_ENABLE no)     # 禁用命令键
set(ONBOARD_CMSIS_DAP yes) # 启用板载调试器

# 添加额外的代码文件引用
# 注意这个文件会被多个项目引用，所以要确保此目标存在
if (TARGET tmk)
    target_sources(tmk PRIVATE ${CMAKE_CURRENT_LIST_DIR}/keymap_plain.c)
endif()

