
set(TMK_COMMON ${TMK_CORE_ROOT}/common)

target_sources(tmk PRIVATE
    ${TMK_COMMON}/host.c
	${TMK_COMMON}/keyboard.c
	${TMK_COMMON}/matrix.c
	${TMK_COMMON}/action.c
	${TMK_COMMON}/action_tapping.c
	${TMK_COMMON}/action_layer.c
	${TMK_COMMON}/action_util.c
	${TMK_COMMON}/print.c
	${TMK_COMMON}/debug.c
	${TMK_COMMON}/util.c
	${TMK_COMMON}/hook.c
)

target_include_directories(tmk PRIVATE 
    ${TMK_COMMON}
    ${SRC_ROOT}
    ${SRC_ROOT}/tmk
)

# Keymap 类型 (Unimap/ActionMap/Keymap)
if(UNIMAP_ENABLE)
    target_sources(tmk PRIVATE ${TMK_COMMON}/unimap.c)
    target_compile_definitions(tmk PUBLIC 
        UNIMAP_ENABLE
        ACTIONMAP_ENABLE
    )
elseif(ACTIONMAP_ENABLE)
    target_sources(tmk PRIVATE ${TMK_COMMON}/actionmap.c)
    target_compile_definitions(tmk PUBLIC 
        ACTIONMAP_ENABLE
    )
else()
    target_sources(tmk PRIVATE ${TMK_COMMON}/keymap.c)
endif()

# 启动附加行为相关（BootMagic/BootCheck)
if(BOOTMAGIC_ENABLE)
    target_sources(tmk PRIVATE
        ${TMK_COMMON}/bootmagic.c
    )
    target_sources(${TARGET}.elf PRIVATE
        ${SRC_ROOT}/keyboard/keyboard_bootmagic.c
        ${SRC_ROOT}/tmk/eeconfig.c)
    target_compile_definitions(tmk PUBLIC BOOTMAGIC_ENABLE)
elseif(BOOTCHECK_ENABLE)
    target_sources(${TARGET}.elf PRIVATE 
        ${SRC_ROOT}/keyboard/keyboard_bootcheck.c
    )
    target_compile_definitions(tmk PUBLIC BOOTCHECK_ENABLE)
endif()

# 鼠标键
if(MOUSEKEY_ENABLE)
    target_sources(tmk PRIVATE ${TMK_COMMON}/mousekey.c)
    target_compile_definitions(tmk PUBLIC 
        MOUSEKEY_ENABLE
        MOUSE_ENABLE
    )
endif()

# 多媒体按键、系统键
if(EXTRAKEY_ENABLE)
    target_compile_definitions(tmk PUBLIC 
        EXTRAKEY_ENABLE
    )
endif()

# 调试用控制台
if(CONSOLE_ENABLE)
    target_compile_definitions(tmk PUBLIC 
        CONSOLE_ENABLE
    )
else()
    target_compile_definitions(tmk PUBLIC 
        NO_PRINT
        NO_DEBUG
    )
endif()

# CommandKey
if(COMMAND_ENABLE)
    target_sources(${TARGET}.elf PRIVATE
        keyboard/keyboard_command.c
    )
    target_compile_definitions(tmk PUBLIC 
        COMMAND_ENABLE
    )
endif()

# NRKO
if(NKRO_ENABLE)
    target_compile_definitions(tmk PUBLIC 
        NKRO_ENABLE
        PROTOCOL_LUFA # HACK: 假装我们用的是LUFA
    )
endif()

# 6KRO
if(USB_6KRO_ENABLE)
    target_compile_definitions(tmk PUBLIC 
        USB_6KRO_ENABLE
    )
endif()

# Keyboard Lock
if(KEYBOARD_LOCK_ENABLE)
    target_compile_definitions(tmk PUBLIC 
        KEYBOARD_LOCK_ENABLE
    )
endif()

# CommandKey
if(BACKLIGHT_ENABLE)
    target_sources(tmk PRIVATE
        ${TMK_COMMON}/backlight.c
    )
    target_compile_definitions(tmk PUBLIC 
        BACKLIGHT_ENABLE
    )
endif()

# 指示是否直接调用存放在存储空间中的Keymap
if (KEYMAP_SECTION_ENABLE)
    target_compile_definitions(tmk PUBLIC 
        KEYMAP_SECTION_ENABLE
    )
endif()

# RISC-V: 假装自己是ARM，以使用部分API
target_compile_definitions(tmk PRIVATE 
    __arm__
)
