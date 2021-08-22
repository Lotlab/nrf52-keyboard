/*
Copyright (C) 2020-2021 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <string.h>

#include "keyboard_evt.h"
#include "nrf_qdec.h"
#include "nrfx_qdec.h"

#include "keyboard_matrix.h"
#include "nrf_gpio.h"

#include "app_timer.h"

#ifdef LOW_POWER_QDEC_TICKS
APP_TIMER_DEF(qdec_timer);
#endif

const nrfx_qdec_config_t qdec_config = {
    .reportper = NRF_QDEC_REPORTPER_10,
    .sampleper = NRF_QDEC_SAMPLEPER_128us,
    .psela = ROTARY_ENCODER_A,
    .pselb = ROTARY_ENCODER_B,
    .pselled = NRF_QDEC_LED_NOT_CONNECTED, // disable
    .dbfen = true,
    .sample_inten = true,
    .interrupt_priority = APP_IRQ_PRIORITY_MID,
};

static int8_t count;
static int8_t last_count;
void decoder_event_handler(nrfx_qdec_event_t event)
{
#ifndef LOW_POWER_QDEC_TICKS
    if (event.type == NRF_QDEC_EVENT_REPORTRDY) {
        count += event.data.report.acc;
        if (event.data.report.acc != 0 && count % 4 == 0) {
            if (count > last_count) {
                matrix_forign_add_oneshot(ROTARY_ENCODER_POS);
            } else {
                matrix_forign_add_oneshot(ROTARY_ENCODER_NEG);
            }
            last_count = count;
        }
    }
#else
    if (event.type == NRF_QDEC_EVENT_SAMPLERDY) {
        if (event.data.sample.value == 0 || event.data.sample.value == 2)
            return;

        count += event.data.sample.value;
        if (count % 4 == 0) {
            if (count > last_count) {
                matrix_forign_add_oneshot(ROTARY_ENCODER_POS);
            } else {
                matrix_forign_add_oneshot(ROTARY_ENCODER_NEG);
            }
            last_count = count;
        }
    }
#endif
}

#ifdef LOW_POWER_QDEC_TICKS
static void decoder_timeout_handler(void* p_context)
{
    nrf_qdec_task_trigger(NRF_QDEC_TASK_START);
}
#endif

/**
 * @brief 初始化解码器
 * 
 */
static void encoder_init()
{
    nrfx_qdec_init(&qdec_config, decoder_event_handler);
#ifdef LOW_POWER_QDEC_TICKS
    nrf_qdec_shorts_enable(QDEC_SHORTS_SAMPLERDY_STOP_Msk);
#endif

    nrf_gpio_cfg_input(qdec_config.psela, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(qdec_config.pselb, NRF_GPIO_PIN_PULLUP);

    nrfx_qdec_enable();

#ifdef LOW_POWER_QDEC_TICKS
    app_timer_create(&qdec_timer, APP_TIMER_MODE_REPEATED, decoder_timeout_handler);
    app_timer_start(qdec_timer, APP_TIMER_TICKS(2), NULL);
#endif
}

static void encoder_event_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_INITED: // 初始化ENCODER
            encoder_init();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_SLEEP: // 蓝牙状态事件
        switch (arg2) {
        case SLEEP_EVT_MANUAL:
        case SLEEP_EVT_AUTO:
            nrf_gpio_cfg_sense_input(ROTARY_ENCODER_A, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
            nrf_gpio_cfg_sense_input(ROTARY_ENCODER_B, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
            break;
        case SLEEP_EVT_MANUAL_NO_WAKEUP:
            nrf_gpio_cfg_default(ROTARY_ENCODER_A);
            nrf_gpio_cfg_default(ROTARY_ENCODER_B);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

EVENT_HANDLER(encoder_event_handler);
