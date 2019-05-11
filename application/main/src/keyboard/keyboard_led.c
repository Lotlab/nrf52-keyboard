#include "keyboard_led.h"

#include "nrf.h"
#include "nrf_gpio.h"
#include "app_timer.h"

#ifdef LED_RGB
#include "low_power_pwm.h"

static low_power_pwm_t led_r;
static low_power_pwm_t led_g;
static low_power_pwm_t led_b;

static void pwm_handler(void * p_context) {
    UNUSED_PARAMETER(p_context);
}

void keyboard_led_rgb_set(uint8_t r, uint8_t g, uint8_t b) {
    low_power_pwm_duty_set(&led_r, r);
    low_power_pwm_duty_set(&led_g, g);
    low_power_pwm_duty_set(&led_b, b);
}

static void keyboard_led_rgb_init() {
    uint32_t err_code;
    low_power_pwm_config_t lppwm_config = LOW_POWER_PWM_DEFAULT_CONFIG(0);

    APP_TIMER_DEF(lpp_timer_0);
    lppwm_config.bit_mask = 1 << LED_RGB_R;
    lppwm_config.p_timer_id = &lpp_timer_0;
    err_code = low_power_pwm_init(&led_r, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_1);
    lppwm_config.bit_mask = 1 << LED_RGB_G;
    lppwm_config.p_timer_id = &lpp_timer_1;
    err_code = low_power_pwm_init(&led_g, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_2);
    lppwm_config.bit_mask = 1 << LED_RGB_B;
    lppwm_config.p_timer_id = &lpp_timer_2;
    err_code = low_power_pwm_init(&led_b, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start(&led_r, led_r.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_g, led_g.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_b, led_b.bit_mask);
    APP_ERROR_CHECK(err_code);

    // turn off rgb led
    keyboard_led_rgb_set(0x00, 0x00, 0x00);
}
#endif

void keyboard_led_init()
{
#ifdef LED_RGB
    keyboard_led_rgb_init();
#endif

#ifdef LED_NUM
    nrf_gpio_cfg_output(LED_NUM);
#endif
#ifdef LED_CAPS
    nrf_gpio_cfg_output(LED_CAPS);
#endif
#ifdef LED_SCLK
    nrf_gpio_cfg_output(LED_SCLK);
#endif
}

void keyboard_led_set(uint8_t led_val) {
#ifdef LED_NUM
    nrf_gpio_pin_write(LED_NUM, led_val && (1 << 0));
#endif
#ifdef LED_CAPS
    nrf_gpio_pin_write(LED_CAPS, led_val && (1 << 1));
#endif
#ifdef LED_SCLK
    nrf_gpio_pin_write(LED_SCLK, led_val && (1 << 2));
#endif
}