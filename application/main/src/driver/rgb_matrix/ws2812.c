/*
Copyright 2018 Sekigon   modify by Geno

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

#include "ws2812.h"
#include "config.h"
#include "nrf_drv_pwm.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrfx_power.h"
#include "usb_comm.h"
#include "wait.h"

#define PWM_PERIOD 18
#define PWM_0H_DURATION 5
#define PWM_1H_DURATION 13

nrf_drv_pwm_t pwm0 = NRFX_PWM_INSTANCE(0);

#define DUMMY_SIGNAL_LEN 10

void pwm_handler(nrfx_pwm_evt_type_t event_type)
{
}

void ws2812_setleds(LED_TYPE* ledarray, uint16_t number_of_leds)
{
    // if (usb_working()) {
    ws2812_setleds_pin(ledarray, number_of_leds, RGB_DI_PIN);
    // }
}

void ws2812_setleds_pin(LED_TYPE* ledarray, uint16_t number_of_leds, uint8_t pinmask)
{
    static bool flag_init;
    if (!flag_init) {
        nrfx_pwm_config_t pwm0_config = {
            .output_pins = {
                pinmask, // channel 0
                NRF_DRV_PWM_PIN_NOT_USED, // channel 1
                NRF_DRV_PWM_PIN_NOT_USED, // channel 2
                NRF_DRV_PWM_PIN_NOT_USED // channel 3
            },
            .irq_priority = APP_IRQ_PRIORITY_LOW,
            .base_clock = NRF_PWM_CLK_16MHz,
            .count_mode = NRF_PWM_MODE_UP,
            .top_value = PWM_PERIOD,
            .load_mode = NRF_PWM_LOAD_COMMON,
            .step_mode = NRF_PWM_STEP_AUTO
        };
        nrfx_pwm_init(&pwm0, &pwm0_config, pwm_handler);
        flag_init = true;
    }

    const uint16_t t0H = PWM_0H_DURATION | (0x8000);
    const uint16_t t1H = PWM_1H_DURATION | (0x8000);
    nrf_pwm_values_common_t led[RGBLED_NUM * 3 * 8 + DUMMY_SIGNAL_LEN * 3 * 8 + 8];
    nrf_pwm_values_t p_led = { .p_common = led };
    nrf_pwm_sequence_t pwm_seq = {
        .values = p_led,
        .length = number_of_leds * 3 * 8 + DUMMY_SIGNAL_LEN * 3 * 8 + 8,
        .repeats = 0,
        .end_delay = 0,
    };
    uint16_t* p_dat = &led[0];

    for (int i = 0; i < number_of_leds; i++) {
        uint8_t mask = 0x80;
        for (int j = 0; j < 8; j++) {
            *p_dat++ = (ledarray[i].g & (mask >> j)) ? t1H : t0H;
        }
        mask = 0x80;
        for (int j = 0; j < 8; j++) {
            *p_dat++ = (ledarray[i].r & (mask >> j)) ? t1H : t0H;
        }
        mask = 0x80;
        for (int j = 0; j < 8; j++) {
            *p_dat++ = (ledarray[i].b & (mask >> j)) ? t1H : t0H;
        }
    }

    for (int i = 0; i < DUMMY_SIGNAL_LEN; i++) {
        for (int j = 0; j < 24; j++) {
            *p_dat++ = t0H;
        }
    }
    // reset signal
    *p_dat++ = 0x8000;
    *p_dat++ = 0x8000;
    *p_dat++ = 0x8000;
    *p_dat++ = 0x8000;
    *p_dat++ = 0x8000;
    *p_dat++ = 0x8000;
    *p_dat++ = 0x8000;
    *p_dat++ = 0x8000;
    nrfx_pwm_simple_playback(&pwm0, &pwm_seq, 1, NRFX_PWM_FLAG_STOP);
    //  wait_us(50);
}

void ws2812_setleds_rgbw(LED_TYPE* ledarray, uint16_t number_of_leds)
{
    const uint16_t t0H = ((int)(0.35f / 0.0625f)) | (0x8000);
    const uint16_t t1H = ((int)(1.36f / 0.0625f)) | (0x8000);
    nrf_pwm_values_common_t led[RGBLED_NUM * 3 * 8 + 1];
    nrf_pwm_values_t p_led = { .p_common = led };
    nrf_pwm_sequence_t pwm_seq = {
        .values = p_led,
        .length = sizeof(led)
            / sizeof(led[0]),
        .repeats = 0,
        .end_delay = 0,
    };
    uint16_t* p_dat = &led[0];

    for (int i = 0; i < number_of_leds; i++) {
        uint8_t mask = 1;
        for (int j = 0; j < 8; j++) {
            *p_dat++ = (ledarray[i].r & (mask << j)) ? t1H : t0H;
        }
        mask = 1;
        for (int j = 0; j < 8; j++) {
            *p_dat++ = (ledarray[i].g & (mask << j)) ? t1H : t0H;
        }
        mask = 1;
        for (int j = 0; j < 8; j++) {
            *p_dat++ = (ledarray[i].b & (mask << j)) ? t1H : t0H;
        }
    }
    *p_dat++ = 0x8000;
    nrfx_pwm_simple_playback(&pwm0, &pwm_seq, 1, NRFX_PWM_FLAG_STOP);
}

//初始化RGB power控制脚
void ws2812_pwr_init(void)
{
#ifdef RGB_PWR_PIN
    nrf_gpio_cfg_output(RGB_PWR_PIN);
#endif
#ifdef RGB_PWR_PIN_REVERSE
    nrf_gpio_cfg_output(RGB_PWR_PIN_REVERSE);
#endif
}

//释放RGB power控制脚
void ws2812_pwr_deinit(void) 
{
    nrf_gpio_cfg_default(RGB_DI_PIN);
#ifdef RGB_PWR_PIN
    nrf_gpio_cfg_default(RGB_PWR_PIN);
#endif
#ifdef RGB_PWR_PIN_REVERSE
    nrf_gpio_cfg_default(RGB_PWR_PIN_REVERSE);
#endif
}

//开启RGB电源
void ws2812_pwr_on(void)
{
#ifdef RGB_PWR_PIN
    nrf_gpio_pin_write(RGB_PWR_PIN, 0);
#endif
#ifdef RGB_PWR_PIN_REVERSE
    nrf_gpio_pin_write(RGB_PWR_PIN_REVERSE, 1);
#endif
}

//关闭RGB电源
void ws2812_pwr_off(void)
{
#ifdef RGB_PWR_PIN
    nrf_gpio_pin_write(RGB_PWR_PIN, 1);
#endif
#ifdef RGB_PWR_PIN_REVERSE
    nrf_gpio_pin_write(RGB_PWR_PIN_REVERSE, 0);
#endif
}