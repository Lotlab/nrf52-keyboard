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

#include "adc_convert.h"

#include "app_timer.h"
#include "nrf_saadc.h"
#include "nrfx_saadc.h"

NRF_SECTION_DEF(adc_channel, struct adc_channel_config*);

#define TOTAL_SAMPLES_BUFFER 16
// #define SINGLE_CHANNEL_BUFFER_LEN (TOTAL_SAMPLES_BUFFER / ADC_CONFIG_COUNT)
#define SINGLE_CHANNEL_BUFFER_LEN 1
#define SAMPLES_IN_BUFFER (SINGLE_CHANNEL_BUFFER_LEN * ADC_CONFIG_COUNT)
static nrf_saadc_value_t m_buffer_pool[2][TOTAL_SAMPLES_BUFFER];

#define ADC_TIMER_PERIOD 10
#define ADC_TIMER_INTERVAL APP_TIMER_TICKS(ADC_TIMER_PERIOD)
APP_TIMER_DEF(adc_timer);

static bool saadc_inited = false;

/**
 * @brief ADC 的事件回调
 * 
 * @param p_event 
 */
static void adc_event_callback(nrfx_saadc_evt_t const* p_event)
{
    nrf_saadc_value_t results[8] = { 0 };

    if (p_event->type == NRFX_SAADC_EVT_DONE) {
        ret_code_t err_code;
        // reload Buffer
        err_code = nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);

        // 处理数据
        int vars_cnt = ADC_CONFIG_COUNT;
        for (uint8_t i = 0; i < vars_cnt; i++) {
            for (uint8_t j = i; j < SAMPLES_IN_BUFFER; j += vars_cnt) {
                results[i] += p_event->data.done.p_buffer[j];
            }
            results[i] /= SINGLE_CHANNEL_BUFFER_LEN;
        }

        // 然后将各个通道的结果发送给接收者
        for (int i = 0; i < vars_cnt; i++) {
            struct adc_channel_config* channel = ADC_CONFIG_GET(i);
            if (channel->adc_finish != 0) {
                if (channel->period_pass <= ADC_TIMER_PERIOD) {
                    channel->adc_finish(results[i]);
                    // reload 周期
                    channel->period_pass = channel->period;
                }
            }
        }

        // 关闭saadc节省电量
        nrfx_saadc_uninit();
        saadc_inited = false;
    }
}

static const nrfx_saadc_config_t config = NRFX_SAADC_DEFAULT_CONFIG;

/**
 * @brief 初始化SAADC
 * 
 */
static void saadc_init()
{
    ret_code_t err_code;
    err_code = nrfx_saadc_init(&config, adc_event_callback);
    APP_ERROR_CHECK(err_code);

    // 注册通道
    int vars_cnt = ADC_CONFIG_COUNT;
    for (int i = 0; i < vars_cnt; i++) {
        struct adc_channel_config* channel = ADC_CONFIG_GET(i);
        err_code = nrfx_saadc_channel_init(i, channel->config);
        APP_ERROR_CHECK(err_code);
    }

    // 注册buffer
    err_code = nrfx_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief ADC 启动转换
 * 
 * @param context 
 */
static void adc_convert(void* context)
{
    bool should_start = false;

    // 通知ADC即将开始转换
    int vars_cnt = ADC_CONFIG_COUNT;
    for (int i = 0; i < vars_cnt; i++) {
        struct adc_channel_config* channel = ADC_CONFIG_GET(i);
        if (channel->period_pass <= ADC_TIMER_PERIOD) {
            should_start = true;
            if (channel->adc_start != 0)
                channel->adc_start();
        } else {
            channel->period_pass -= ADC_TIMER_PERIOD;
        }
    }

    if (should_start) {
        // 如果没有初始化，则尝试初始化
        if (!saadc_inited) {
            saadc_init();
            saadc_inited = true;
        }

        // 转换ADC
        nrfx_saadc_sample();
    }
}

/**
 * @brief 启动ADC计时器
 * 
 */
void adc_timer_start()
{
    ret_code_t err_code;

    err_code = app_timer_start(adc_timer, ADC_TIMER_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 初始化ADC
 * 
 */
void adc_init()
{
    ret_code_t err_code;

    // 创建ADC转换器的计时器
    err_code = app_timer_create(&adc_timer, APP_TIMER_MODE_REPEATED, adc_convert);
    APP_ERROR_CHECK(err_code);
}
