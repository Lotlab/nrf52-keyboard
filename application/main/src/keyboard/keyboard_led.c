#include "keyboard_led.h"
#include "low_power_pwm.h"
#include "../config/keyboard_config.h"

#include "nrf.h"
#include "app_timer.h"

static low_power_pwm_t led_r;
static low_power_pwm_t led_g;
static low_power_pwm_t led_b;

static void pwm_handler(void * p_context) {
    UNUSED_PARAMETER(p_context);
}

void keyboard_led_set(uint8_t r, uint8_t g, uint8_t b) {
    low_power_pwm_duty_set(&led_r, r);
    low_power_pwm_duty_set(&led_g, g);
    low_power_pwm_duty_set(&led_b, b);
}

void keyboard_led_init() {
    uint32_t err_code;
    low_power_pwm_config_t lppwm_config = LOW_POWER_PWM_DEFAULT_CONFIG(0);

    APP_TIMER_DEF(lpp_timer_0);
    lppwm_config.bit_mask = 1 << LED_EXT1;
    lppwm_config.p_timer_id = &lpp_timer_0;
    err_code = low_power_pwm_init(&led_r, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_1);
    lppwm_config.bit_mask = 1 << LED_EXT2;
    lppwm_config.p_timer_id = &lpp_timer_1;
    err_code = low_power_pwm_init(&led_g, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_2);
    lppwm_config.bit_mask = 1 << LED_EXT3;
    lppwm_config.p_timer_id = &lpp_timer_2;
    err_code = low_power_pwm_init(&led_b, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start(&led_r, led_r.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_g, led_g.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_b, led_b.bit_mask);
    APP_ERROR_CHECK(err_code);

    keyboard_led_set(0xFF, 0xFF, 0x00);
}
