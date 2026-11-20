# Lot60-BLE 键盘

## 描述

- 5x14的60%键盘
- 一个Caps灯和一个状态指示灯
- Type-C接口

## 硬件

参见 [BLE60-keyboard](https://github.com/Lotlab/BLE60-keyboard)的 type-c 分支。

## 默认行为

在没有USB接入时，按键空闲15秒进入慢速扫描状态，600秒自动进入休眠。
自动休眠后按任意按键即可唤醒；手动休眠后需要按开机按钮开机。

插入USB也可以唤醒设备。

- Space+U 开机
- Space+E 清空绑定设备
- Fn+Caps 休眠
- Fn+Tabs 切换USB/蓝牙

## 默认指示灯颜色表

- 白色<span style="color: #FFFFFF">■</span>：无连接
- 青色<span style="color: #66FFFF">■</span>：蓝牙已连接
- 天蓝色<span style="color: #0099FF">■</span>：USB已连接
- 橙色<span style="color: #FF8000">■</span>：充电中
- 绿色<span style="color: #00FF00">■</span>：充电完毕
- 黄色<span style="color: #FFFF00">■</span>：输入配对密码
- 紫红色<span style="color: #FF0080">■</span>：配对密码输入完毕
- 紫色<span style="color: #FF00FF">■</span>：休眠

---

## v1.2.1.0 Fork Notes (English)

### Default BLE Multi-Device Shortcuts (`keymap_plain.c`)
- `Fn + Tab`: Toggle USB / BLE mode (`0xF001`)
- `Fn + CapsLock`: Manual deep sleep (`0xF000`)
- `Fn + X`: Toggle NKRO (`0xF100`)
- `Fn + R`: Clear / Re-bond current BLE slot (`0xF701`)
- `Fn + T` / `Fn + Y` / `Fn + U`: Switch BLE Host Profile `0` / `1` / `2` (`0xF801` / `0xF901` / `0xFA01`)

### Optimizations & Fixes in `v1.2.1.0` (`nRF52810` / `rev_g`)
- **RAM Safety & Headroom**:
  - Fixed UART packet buffer overflow (`recv_buf[64]`) and `recv_len - 4` underflow in `usb_comm.c`.
  - Fixed keymap layer bounds check (`layer >= MAX_LAYER`) and macro boundary guards in `data_storage.c` / `macro_player.c`.
  - Set `HEAP_SIZE := 0` in `nrf52810.mk` (reclaiming 2 KB RAM on nRF52810).
- **Battery & BLE Power**:
  - Reduced `ADC_TIMER_PERIOD` from `10 ms` to `1000 ms` (-99 CPU wakeups/sec) and battery polling period from `2 s` to `10 s`.
  - Enabled `DYNAMIC_TX_POWER` (disabling fixed +4 dBm `HIGH_TX_POWER`) and fixed `m_conn_handle` ordering in `BLE_GAP_EVT_CONNECTED`.
  - Tuned BLE connection parameters (`MIN_CONN_INTERVAL=15 ms`, `MAX_CONN_INTERVAL=30 ms`, `SLAVE_LATENCY=15`, `CONN_SUP_TIMEOUT=4000 ms`).
  - Configured matrix row GPIOs once in `matrix_init()` with standard drive (`NRF_GPIO_PIN_S0S1`) and single-register `NRF_P0->IN` reads in `keyboard_matrix.c`.
- **Fresh-Flash Default Auto-Load**:
  - When FDS EEPROM is empty (after SWD flash or factory reset), `storage_read()` automatically loads `keymap_plain.c` defaults and `config.h` timeouts (`600 s` sleep, `15 s` slow-scan, `5 s` LED auto-off) so HID configurators immediately read a valid default profile.
- **GCC 12+ Compatibility**:
  - Added `-Wno-array-bounds` in `app.mk` for Nordic nRF5 SDK 15.3 `nrf_section.h` compatibility with modern `arm-none-eabi-gcc`.
