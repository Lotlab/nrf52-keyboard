#include <stdint.h>

#include "nrf_section.h"
#define __STATIC_INLINE static inline
#include "nrfx_saadc.h"

struct adc_channel_config {
    /**
     * @brief 通道配置
     * 
     */
    nrf_saadc_channel_config_t* config;
    /**
     * @brief ADC准备启动的事件
     * 
     */
    void (*adc_start)(void);
    /**
     * @brief ADC完毕的事件
     * 
     */
    void (*adc_finish)(nrf_saadc_value_t);
    /**
     * @brief 周期
     * 
     */
    uint16_t period;

    /**
     * @brief 计数用周期
     * 
     */
    uint16_t period_pass;
};

#define ADC_CONFIG_COUNT NRF_SECTION_ITEM_COUNT(adc_channel, struct adc_channel_config*)
#define ADC_CONFIG_GET(i) (*(NRF_SECTION_ITEM_GET(adc_channel, struct adc_channel_config*, i)))
#define ADC_CONVERT_CHANNEL(conf) NRF_SECTION_ITEM_REGISTER(adc_channel, const struct adc_channel_config* _pt_##conf) = &conf

void adc_timer_start(void);
void adc_init(void);