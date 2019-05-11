#include "keyboard_led.h"

#include "app_timer.h"
#include "nrf.h"
#include "nrf_gpio.h"

#ifdef LED_RGB
#include "low_power_pwm.h"

static low_power_pwm_t led_r;
static low_power_pwm_t led_g;
static low_power_pwm_t led_b;
static uint32_t saved_color;

static void off_timer_start();

static void pwm_handler(void* p_context)
{
    UNUSED_PARAMETER(p_context);
}

/**
 * @brief 设置RGB灯光值
 * 
 * @param color 
 */
static void keyboard_led_rgb_set_internal(uint32_t color)
{
    low_power_pwm_duty_set(&led_r, (color && 0xFF0000) >> 16);
    low_power_pwm_duty_set(&led_g, (color && 0xFF00) >> 8);
    low_power_pwm_duty_set(&led_b, (color && 0xFF));
}

/**
 * @brief 设置RGB灯光值并启用自动熄灭
 * 
 * @param color 
 */
void keyboard_led_rgb_set(uint32_t color)
{
    saved_color = color;
    keyboard_led_rgb_set_internal(color);
    off_timer_start();
}

/**
 * @brief 初始化RGB灯光
 * 
 */
static void keyboard_led_rgb_init()
{
    uint32_t err_code;
    low_power_pwm_config_t lppwm_config = LOW_POWER_PWM_DEFAULT_CONFIG(0);

    APP_TIMER_DEF(lpp_timer_0);
    lppwm_config.bit_mask = 1 << LED_RGB_R;
    lppwm_config.p_timer_id = &lpp_timer_0;
    err_code = low_power_pwm_init(&led_r, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_r, 0);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_1);
    lppwm_config.bit_mask = 1 << LED_RGB_G;
    lppwm_config.p_timer_id = &lpp_timer_1;
    err_code = low_power_pwm_init(&led_g, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_g, 0);
    APP_ERROR_CHECK(err_code);

    APP_TIMER_DEF(lpp_timer_2);
    lppwm_config.bit_mask = 1 << LED_RGB_B;
    lppwm_config.p_timer_id = &lpp_timer_2;
    err_code = low_power_pwm_init(&led_b, &lppwm_config, pwm_handler);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_duty_set(&led_b, 0);
    APP_ERROR_CHECK(err_code);

    err_code = low_power_pwm_start(&led_r, led_r.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_g, led_g.bit_mask);
    APP_ERROR_CHECK(err_code);
    err_code = low_power_pwm_start(&led_b, led_b.bit_mask);
    APP_ERROR_CHECK(err_code);
}
#endif

/**
 * @brief 设置LED灯的值
 * 
 * @param led_val 
 */
static void keyboard_led_set_internal(uint8_t led_val)
{
#ifdef LED_NUM
    LED_WRITE(LED_NUM, led_val && (1 << 0));
#endif
#ifdef LED_CAPS
    LED_WRITE(LED_CAPS, led_val && (1 << 1));
#endif
#ifdef LED_SCLK
    LED_WRITE(LED_SCLK, led_val && (1 << 2));
#endif
}

static uint8_t saved_led_val;

/**
 * @brief 设置LED灯的值步并启用自动熄灭
 * 
 * @param led_val 
 */
void keyboard_led_set(uint8_t led_val)
{
    saved_led_val = led_val;
    keyboard_led_set_internal(led_val);
    off_timer_start();
}

/**
 * @brief 将LED点亮
 * 
 */
static void led_on(void)
{
    keyboard_led_set_internal(saved_led_val);
#ifdef LED_RGB
    keyboard_led_rgb_set_internal(saved_color);
#endif
}

/**
 * @brief 将LED熄灭
 * 
 */
static void led_off(void)
{
    keyboard_led_set_internal(0);
#ifdef LED_RGB
    keyboard_led_rgb_set_internal(0);
#endif
}

/**
 * @brief 将LED熄灭
 * 
 */
void keyboard_led_off(void)
{
    led_off();
}

#if LED_AUTOOFF_TIME > 0

static bool counting;
static bool led_autooff = true;
APP_TIMER_DEF(led_off_timer);

/**
 * @brief LED自动关闭的handler
 * 
 * @param context 
 */
static void led_off_timer_handler(void* context)
{
    led_off();
    counting = false;
}

/**
 * @brief 启动自动关闭计时器
 * 
 */
static void off_timer_start()
{
    if (led_autooff) {
        if (counting)
            app_timer_stop(led_off_timer);
        app_timer_start(led_off_timer, APP_TIMER_TICKS(LED_AUTOOFF_TIME), NULL);
        counting = true;
    }
}

/**
 * @brief 初始计时器
 * 
 */
static void off_timer_init(void)
{
    app_timer_create(&led_off_timer, APP_TIMER_MODE_SINGLE_SHOT, led_off_timer_handler);
}

/**
 * @brief 设置省电模式状态
 * 
 * @param powersave 
 */
void keyboard_led_powersave(bool powersave)
{
    if (counting)
        app_timer_stop(led_off_timer);

    led_autooff = powersave;
    led_on();
}

#else

static void off_timer_init()
{
}

static void off_timer_start()
{
}

void keyboard_led_powersave(bool powersave)
{
}

#endif

/**
 * @brief 初始化LED各项设置
 * 
 */
void keyboard_led_init()
{
#ifdef LED_RGB
    keyboard_led_rgb_init();
#endif
    off_timer_init();

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
