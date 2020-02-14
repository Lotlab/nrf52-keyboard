/*
Copyright 2011 Jun Wako <wakojun@gmail.com>
Modified by Geno <geno@live.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "keyboard_command.h"
#include "action_layer.h"
#include "action_util.h"
#include "app_timer.h"
#include "ble_services.h"
#include "bootloader.h"
#include "command.h"
#include "host.h"
#include "keyboard.h"
#include "keycode.h"
#include "keymap.h"
#include "main.h"
#include "nrf_delay.h"
#include "nrf_pwr_mgmt.h"
#ifdef RGBLIGHT_ENABLE
#include "rgblight.h"
#endif
#include "eeconfig.h"
#include "usb_comm.h"
#include "util.h"
#include "wait.h"
#include <stdbool.h>
#include <stdint.h>

APP_TIMER_DEF(command_run_timer);
#ifdef MULTI_DEVICE_SWITCH
uint8_t devices_id = 0;
bool restart_without_whitelist = false;
#endif
/**
 * COMMAND延迟处理事件
 */
enum command_delay_hander_event {
    COMMAND_DFU,        //跳转Bootloader
    COMMAND_SLEEP,      //手动休眠
    COMMAND_RESET,      //重置键盘
    COMMAND_BOND,       //清空绑定
    COMMAND_SWITCH     //切换蓝牙设备
};

/**
 * @brief 延迟运行handler
 * 
 * @param context 
 */
static void command_delay_handler(void* p_context)
{
    uint8_t handler_name = (uint32_t)p_context;
    switch (handler_name) {
    case COMMAND_BOND:
#ifdef MULTI_DEVICE_SWITCH
        switch_device_rebond();
        restart_without_whitelist = true;
        advertising_restart(BLE_ADV_MODE_FAST, restart_without_whitelist);
#else
        delete_bonds();
#endif
        break;
    case COMMAND_SWITCH:
#ifdef MULTI_DEVICE_SWITCH
        switch_device_select(devices_id);
        restart_without_whitelist = false;
        advertising_restart(BLE_ADV_MODE_FAST, restart_without_whitelist);
#endif
        break;
    case COMMAND_RESET:
        delete_bonds();
#ifdef BOOTMAGIC_ENABLE
        eeconfig_init();
#endif
#ifdef MULTI_DEVICE_SWITCH
        switch_device_id_write(0);
#endif
        break;
    case COMMAND_SLEEP:
        sleep(SLEEP_MANUALLY);
        break;
    case COMMAND_BOOTLOADER:
        bootloader_jump();
        break;
    default:
        break;
    }
}

/**
 * @brief 初始化运行计时器
 * 
 */
void command_timer_init(void)
{
    app_timer_create(&command_run_timer, APP_TIMER_MODE_SINGLE_SHOT, command_delay_handler);
}

static bool command_common(uint8_t code);

static void switch_default_layer(uint8_t layer);

command_state_t command_state = ONESHOT;

bool command_proc(uint8_t code)
{
    switch (command_state) {
    case ONESHOT:
        if (!IS_COMMAND())
            return false;
        return (command_extra(code) || command_common(code));
        break;
    default:
        command_state = ONESHOT;
        return false;
    }
    return true;
}

/* TODO: Refactoring is needed. */
/* This allows to define extra commands. return false when not processed. */
bool command_extra(uint8_t code) __attribute__((weak));
bool command_extra(uint8_t code)
{
    (void)code;
    return false;
}

static bool command_common(uint8_t code)
{
#ifdef KEYBOARD_LOCK_ENABLE
    static host_driver_t* host_driver = 0;
#endif
    switch (code) {
    case KC_PAUSE:
        //跳转到bootloader
        clear_keyboard();
        bootloader_jump();
        break;
#ifdef NKRO_ENABLE
    case KC_N:
        //切换全键无冲模式
        clear_keyboard();
        keyboard_nkro = !keyboard_nkro;
        break;
#endif
    case KC_0:
    case KC_F10:
        //切换默认层
        switch_default_layer(0);
        break;
    case KC_1 ... KC_9:
        //切换默认层
        switch_default_layer((code - KC_1) + 1);
        break;
    case KC_F1 ... KC_F9:
        //切换默认层
        switch_default_layer((code - KC_F1) + 1);
        break;
    case KC_M:
#ifdef HAS_USB
        //USB与蓝牙输入模式切换
        clear_keyboard();
        usb_comm_switch();
#endif
        break;
    case KC_O:
        //清空绑定数据
        clear_keyboard();
        app_timer_start(command_run_timer, APP_TIMER_TICKS(200), (void*)(uint32_t)COMMAND_BOND);
        break;
#ifdef MULTI_DEVICE_SWITCH //多设备切换：支持3台设备切换
    case KC_Q:
        clear_keyboard();
        devices_id = 0;
        app_timer_start(command_run_timer, APP_TIMER_TICKS(100), (void*)(uint32_t)COMMAND_SWITCH);
        break;
    case KC_W:
        clear_keyboard();
        devices_id = 1;
        app_timer_start(command_run_timer, APP_TIMER_TICKS(100), (void*)(uint32_t)COMMAND_SWITCH);
        break;
    case KC_E:
        clear_keyboard();
        devices_id = 2;
        app_timer_start(command_run_timer, APP_TIMER_TICKS(100), (void*)(uint32_t)COMMAND_SWITCH);
        break;
#endif
    case KC_R:
        clear_keyboard();
        advertising_restart(BLE_ADV_MODE_FAST, true);
        break;
#ifdef RGBLIGHT_ENABLE //RGB灯控制
    case KC_Z:
        rgblight_step();
        break;
    case KC_X:
        rgblight_toggle();
        break;
    case KC_C:
        rgblight_increase_hue();
        break;
    case KC_V:
        rgblight_decrease_hue();
        break;
    case KC_A:
        rgblight_increase_sat();
        break;
    case KC_S:
        rgblight_decrease_sat();
        break;
    case KC_D:
        rgblight_increase_val();
        break;
    case KC_F:
        rgblight_decrease_val();
        break;
#endif
    case KC_B:
        //重启到DFU模式
        clear_keyboard();
#ifdef RGBLIGHT_ENABLE
        rgblight_disable_noeeprom();
#endif
        app_timer_start(command_run_timer, APP_TIMER_TICKS(1000), (void*)(uint32_t)COMMAND_DFU);
        break;
    case KC_P:
        //休眠
        clear_keyboard();
#ifdef RGBLIGHT_ENABLE
        rgblight_disable_noeeprom();
#endif
        app_timer_start(command_run_timer, APP_TIMER_TICKS(1000), (void*)(uint32_t)COMMAND_SLEEP);
        break;
    case KC_I:
        //重置键盘
        clear_keyboard();
        app_timer_start(command_run_timer, APP_TIMER_TICKS(200), (void*)(uint32_t)COMMAND_RESET);
        break;
    default:
        return false;
    }
    return true;
}

static void switch_default_layer(uint8_t layer)
{
    default_layer_set(1UL << layer);
    clear_keyboard();
}
