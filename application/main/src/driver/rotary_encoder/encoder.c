/*
Copyright (C) 2020 Jim Jiang <jim@lotlab.org>

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

#include "keyboard_evt.h"
#include "nrfx_qdec.h"

#include "ssd1306/oled_graph.h"
#include "ssd1306/ssd1306_oled.h"

#include "nrf_gpio.h"

#define ROTARY_ENCODER_A 19
#define ROTARY_ENCODER_B 20

const nrfx_qdec_config_t qdec_config = {
    .reportper = NRF_QDEC_REPORTPER_10,
    .sampleper = NRF_QDEC_SAMPLEPER_512us,
    .psela = ROTARY_ENCODER_A,
    .pselb = ROTARY_ENCODER_B,
    .pselled = NRF_QDEC_LED_NOT_CONNECTED, // disable
    .dbfen = true,
    .sample_inten = false,
    .interrupt_priority = APP_IRQ_PRIORITY_MID,
};

static void decoder_event_handler(nrfx_qdec_event_t event)
{
    if (event.type == NRF_QDEC_EVENT_REPORTRDY) {
        if (event.data.report.acc > 0)
            oled_draw_text_5x8(3, TEXT_ALIGN_CENTER, 0, "POS");
        else if (event.data.report.acc < 0)
            oled_draw_text_5x8(3, TEXT_ALIGN_CENTER, 0, "NEG");
        ssd1306_show_dirty_block();
    } 
}

/**
 * @brief 初始化解码器
 * 
 */
static void encoder_init()
{
    nrfx_qdec_init(&qdec_config, decoder_event_handler);
    nrf_gpio_cfg_input(qdec_config.psela, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(qdec_config.pselb, NRF_GPIO_PIN_PULLUP);

    nrfx_qdec_enable();
}

static void encoder_event_handler(enum user_event event, void* arg)
{
    uint8_t param = (uint32_t)arg;
    if (event == USER_EVT_STAGE && param == KBD_STATE_INITED)
        encoder_init();
}

EVENT_HANDLER(encoder_event_handler);
