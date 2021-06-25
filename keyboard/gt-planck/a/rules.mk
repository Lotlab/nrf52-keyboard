# 配置项：主控芯片类型
# 可选：nrf52810 或 nrf52832
NRF_CHIP := nrf52832

# 配置项：功能选项
# BOOTMAGIC_ENABLE = yes  # 启用Bootmagic
# BOOTCHECK_ENABLE = yes  # 启用Bootchek
EXTRAKEY_ENABLE = yes	  # 启用媒体键功能
MOUSEKEY_ENABLE = yes     # 启用鼠标键功能
NKRO_ENABLE = yes	      # 启用USB的全键无冲功能
USB_6KRO_ENABLE = yes     # 启用USB的六键无冲功能
COMMAND_ENABLE = yes      # 启用调试和配置的命令
ifeq (nrf52832,$(strip $(NRF_CHIP)))
WS2812_ENABLE = yes     #启用WS2812灯
endif
# THREE_LED_STATUS = yes    #启用键盘运行状态灯
ONBOARD_CMSIS_DAP = yes   # 启用板载调试器
# RGB_LIGHT_ENABLE = yes     #启用RGB轴灯
ROTARY_ENCODER = yes # 启用编码器
# SSD1306_OLED = yes
ACTIONMAP_ENABLE =yes
