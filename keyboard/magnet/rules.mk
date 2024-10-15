# 主控芯片类型：可选 nrf52810 或 nrf52832
NRF_CHIP := nrf52832

# 功能选项：更多可配置项目，请参考doc目录下的相应文档
BOOTMAGIC_ENABLE = no   # 启用Bootmagic
BOOTCHECK_ENABLE = no   # 启用Bootcheck
EXTRAKEY_ENABLE = yes   # 启用媒体键功能
MOUSEKEY_ENABLE = yes   # 启用鼠标键功能
USB_6KRO_ENABLE = yes   # 启用USB的六键无冲功能
NKRO_ENABLE = yes       # 启用USB的全键无冲功能
COMMAND_ENABLE = yes    # 启用调试和配置的命令
RGBLIGHT_ENABLE = no    # 启用RGB灯
THREE_LED_STATUS = no   # 启用键盘运行状态灯
ONBOARD_CMSIS_DAP = yes # 启用板载调试器
NRF52_DISABLE_FPU = yes # 禁用FPU
