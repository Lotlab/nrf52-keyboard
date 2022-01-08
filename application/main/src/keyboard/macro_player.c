/*
Copyright 2013 Jun Wako <wakojun@gmail.com>
Copyright 2019 Jim Jiang <jim@lotlab.org>

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

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

#include "action.h"
#include "action_macro_extend.h"
#include "action_util.h"
#include "app_timer.h"
#include "keyboard_host_driver.h"
#include "queue.h"
#include "mousekey.h"

#define MACRO_PLAY_INTERVAL APP_TIMER_TICKS(1)
APP_TIMER_DEF(m_keyboard_macro_timer);

#ifndef NO_ACTION_MACRO
#define MAX_MACRO_QUEUE 5
#define DEFAULT_MACRO_INTERVAL 5

static const macro_t* current_macro = 0;
static uint8_t macro_interval = 0;
static uint8_t macro_interval_reload = DEFAULT_MACRO_INTERVAL;
static uint8_t macro_delay = 0;
static uint8_t mod_storage = 0;
static bool timer_started = false;
static bool macro_repeaet = false;

QUEUE(const macro_t*, macro_queue, MAX_MACRO_QUEUE);

void macro_cancle()
{
    macro_queue_clear();
    current_macro = 0;
    macro_repeaet = false;
    macro_delay = 0;
    macro_interval_reload = DEFAULT_MACRO_INTERVAL;
}

void action_macro_play(const macro_t* macro_p)
{
    // 处理循环宏
    if (macro_repeaet) {
        // 按下了同一个宏按键，则取消宏
        if (macro_p == *macro_queue_peek()) {
            macro_cancle();
        }
        return;
    }

    macro_queue_push(macro_p);
    // 启用播放定时器
    if (!timer_started) {
        ret_code_t err_code = app_timer_start(
            m_keyboard_macro_timer,
            MACRO_PLAY_INTERVAL,
            NULL);
        APP_ERROR_CHECK(err_code);

        timer_started = true;
    }
}

/**
 * 播放宏
 */
static void action_macro_replay(void* p_context)
{
    UNUSED_PARAMETER(p_context);

    // 等待WAIT命令的毫秒数
    if (macro_delay >= 1) {
        macro_delay -= 1;
        return;
    }
    // 等待设置的两个宏之间的间隔秒数
    if (macro_interval >= 1) {
        macro_interval -= 1;
        return;
    }

    // 重载下一个等待播放的宏
    if (!current_macro) {
        if (macro_queue_empty()) {
            if (timer_started) {
                ret_code_t err_code = app_timer_stop(m_keyboard_macro_timer);
                APP_ERROR_CHECK(err_code);

                timer_started = false;
            }
            return;
        } else {
            current_macro = *macro_queue_peek();
        }
    }

    // 当前发送队列不为空，等待空
    if (!keys_queue_empty())
        return;

    // 播放宏
    switch (*current_macro) {
    case KEY_DOWN:
        current_macro++;
        if (IS_MOD(*current_macro)) {
            add_weak_mods(MOD_BIT(*current_macro));
            send_keyboard_report();
#ifdef MOUSEKEY_ENABLE
        } else if (IS_MOUSEKEY(*current_macro)) {
            mousekey_on(*current_macro);
            mousekey_send();
#endif
        } else {
            register_code(*current_macro);
        }
        current_macro++;
        break;
    case KEY_UP:
        current_macro++;
        if (IS_MOD(*current_macro)) {
            del_weak_mods(MOD_BIT(*current_macro));
            send_keyboard_report();
#ifdef MOUSEKEY_ENABLE
        } else if (IS_MOUSEKEY(*current_macro)) {
            mousekey_off(*current_macro);
            mousekey_send();
#endif
        } else {
            unregister_code(*current_macro);
        }
        current_macro++;
        break;
    case WAIT:
        macro_delay = *++current_macro;
        current_macro++;
        break;
    case INTERVAL:
        macro_interval_reload = *++current_macro;
        current_macro++;
        break;
    case MOD_STORE:
        mod_storage = get_mods();
        current_macro++;
        break;
    case MOD_RESTORE:
        set_mods(mod_storage);
        send_keyboard_report();
        current_macro++;
        break;
    case MOD_CLEAR:
        clear_mods();
        send_keyboard_report();
        current_macro++;
        break;
    case 0x04 ... 0x73:
        register_code(*current_macro);
        current_macro++;
        break;
    case 0x84 ... 0xF3:
        unregister_code(*current_macro & 0x7F);
        current_macro++;
        break;
    case REPEAT:
        // 重复宏：直接重放这个宏
        current_macro = *macro_queue_peek();
        macro_repeaet = true;
        break;
    case END:
        // 出队，重置当前宏的设置
        macro_queue_pop();
        current_macro = 0;
        macro_repeaet = false;
        macro_interval_reload = DEFAULT_MACRO_INTERVAL;
        mod_storage = 0;
        break;
    default:
        return;
    }
    macro_interval = macro_interval_reload;
}

void macro_play_timer_init()
{
    ret_code_t err_code = app_timer_create(
        &m_keyboard_macro_timer,
        APP_TIMER_MODE_REPEATED,
        action_macro_replay);
    APP_ERROR_CHECK(err_code);
}

#endif
