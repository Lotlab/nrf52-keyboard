#include "a800_led.h"
#include "app_timer.h"
#include "config.h"

#include "data_storage.h"
#include "keyboard_evt.h"
#include "keyboard_led.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "power_save.h"

static void write_led_data(uint8_t data);
static void a800_led_blink_led(bool toggle);

// A800 LED驱动PIN脚配置
#ifndef A800_LED_DATA
#define A800_LED_DATA 5    // LED数据pin定义(74HC595 DS)
#endif
#ifndef A800_LED_CLK
#define A800_LED_CLK 3     // LED时钟pin定义(74HC595 SHCP)
#endif
#ifndef A800_LED_ON
#define A800_LED_ON 4      // LED使能pin定义(74HC595 STCP)
#endif

static uint8_t led_data = 0;
// LED通道配置
#define LED_CHANNEL_NUM 5
#define CAP_CHANNEL 0
#define NUM_CHANNEL 1
#define G_CHANNEL 2
#define R_CHANNEL 3
#define B_CHANNEL 4
#define CAP_MASK (1 << CAP_CHANNEL)
#define NUM_MASK (1 << NUM_CHANNEL)
#define R_MASK (1 << R_CHANNEL)
#define G_MASK (1 << G_CHANNEL)
#define B_MASK (1 << B_CHANNEL)
// BLE, USB, CHARGING状态指示灯复用RGB指示灯
#define BLE_CHANNEL B_CHANNEL
#define USB_CHANNEL R_CHANNEL
#define CHARGING_CHANNEL NUM_CHANNEL
#define BLE_MASK (1 << BLE_CHANNEL)
#define USB_MASK (1 << USB_CHANNEL)
#define CHARGING_MASK (1 << CHARGING_CHANNEL)

// LED指示灯闪烁控制
#define A800_LED_BLINK_INTERVAL APP_TIMER_TICKS(100)    // 闪烁速度(ms)
APP_TIMER_DEF(a800_led_blink_timer); // 闪烁定时器
static bool blink_enable = false;    // 开启闪烁
static uint8_t blink_mask = 0;       // 需要闪烁的灯对应位置1
static uint8_t blink_stage = 0xff;   // 闪烁阶段控制. 对应位置1为亮灯阶段, 对应位置0为关灯阶段
static uint32_t blink_mult = 0U;     // 通过倍频方式控制闪烁频率. 每5位为一个通道, 每个通道最大倍频取值0x1f
static uint32_t blink_mult_counting = 0U;   // 倍频计数器
#define MAKE_BLINK_MULT(CHANNEL, VAL) ((VAL & 0x1f) << (CHANNEL * LED_CHANNEL_NUM))
#define GET_BLINK_MULT(CHANNEL, MULT) ((MULT >> (CHANNEL * LED_CHANNEL_NUM)) & 0x1f)

static uint8_t ble_device_id;

static inline void delay_us(uint8_t us)
{
    #ifdef __GNUC__
    #define __nop() __asm("NOP")
    #endif
    // //64mhz, 64cycle = 1us
    for (int i = 0; i < 64 * us; i++) {
        __nop();
    }
}


// static uint8_t saved_a800_led_val; // 暂存的LED的值

// //注册指示灯开关需要的存储区
// CONFIG_SECTION(leds_turn_off, 1);
// /**
//  * 读取状态灯开关存储值.
//  */
// uint8_t leds_turn_off_read(void)
// {
//     return leds_turn_off.data[0];
// }
// /**
//  * 写入状态灯开关存储值
//  */
// void leds_turn_off_write(uint8_t val)
// {
//     if (leds_turn_off.data[0] != val) {
//         leds_turn_off.data[0] = val;
//         storage_write((1 << STORAGE_CONFIG));
//     }
// }

// enum a800_led {
//     BIT_LED_BLE,
//     BIT_LED_USB,
//     BIT_LED_CHARGING
// };

/** 
 * 闪烁定时处理函数
 **/
static void a800_led_blink_timer_handler(void* context)
{
    if (blink_enable)
    {
    a800_led_blink_led(true);
    }
    else
    {
        // 下一次闪烁从亮灯阶段开始
        blink_stage = 0xff;

        // app_timer_stop(a800_led_blink_timer);
    }

    // 每2秒触发一次ADC转换操作.
    static uint8_t n = 0;
    if (++n * A800_LED_BLINK_INTERVAL > 2000)
    {
        nrf_gpio_cfg_default(A800_LED_DATA);
        trig_event_param(USER_EVT_START_ADC, 0);
        n = 0;
    }
}

/** 
 * 更新闪烁状态
 **/
static void a800_led_blink_led(bool toggle)
{
    if (blink_enable == false) return;

    uint8_t data = led_data;

    data &= ~(blink_stage & blink_mask);
    data |= blink_stage & blink_mask;

    write_led_data(data);

    // 更新各通道计算器, 并处理闪烁状态翻转
    for (uint8_t i = 0; i < LED_CHANNEL_NUM; ++i)
    {
        if ((1 << i) & blink_mask)
        {
            // 设置通道倍频值及计数器值
            uint32_t mult = GET_BLINK_MULT(i, blink_mult);
            uint32_t count = GET_BLINK_MULT(i, blink_mult_counting);
            ++count;
            
            // 计算器值小于倍频设置, 则更新通道计数器
            if (count < mult)
            {
                blink_mult_counting &= ~(MAKE_BLINK_MULT(i, 0xff));
                blink_mult_counting |= MAKE_BLINK_MULT(i, count);
            }

            // 计算器值等于倍频设置, 则翻转通道状态
            else if (toggle)
            {
                blink_mult_counting &= ~(MAKE_BLINK_MULT(i, 0xff));
                blink_stage ^= (1 << i);
            }
        }
    }
}

/** 
 * 初始化状态LED
 **/
void a800_led_init()
{
    nrf_gpio_cfg_output(A800_LED_DATA);
    nrf_gpio_cfg_output(A800_LED_CLK);
    nrf_gpio_cfg_output(A800_LED_ON);
    led_data = 0;
    a800_led_off();
}

/**
 * 释放LED
 * 
 */
void a800_led_deinit(void)
{
    a800_led_off();
    nrf_gpio_cfg_default(A800_LED_DATA);
    nrf_gpio_cfg_default(A800_LED_CLK);
    nrf_gpio_cfg_default(A800_LED_ON);
}

/** 
 * 内部设置状态LED的值
 **/
static void write_led_data(uint8_t data)
{
    nrf_gpio_cfg_output(A800_LED_DATA);

    // write bit 4
    nrf_gpio_pin_write(A800_LED_CLK,0);
    nrf_gpio_pin_write(A800_LED_DATA, ((data >> 4) & 0x01));
    delay_us(8);
    nrf_gpio_pin_write(A800_LED_CLK,1);
    delay_us(8);

    // write bit 3
    nrf_gpio_pin_write(A800_LED_CLK,0);
    nrf_gpio_pin_write(A800_LED_DATA, ((data >> 3) & 0x01));
    delay_us(8);
    nrf_gpio_pin_write(A800_LED_CLK,1);
    delay_us(8);

    // write bit 2
    nrf_gpio_pin_write(A800_LED_CLK,0);
    nrf_gpio_pin_write(A800_LED_DATA, ((data >> 2) & 0x01));
    delay_us(8);
    nrf_gpio_pin_write(A800_LED_CLK,1);
    delay_us(8);

    // write bit 1
    nrf_gpio_pin_write(A800_LED_CLK,0);
    nrf_gpio_pin_write(A800_LED_DATA, ((data >> 1) & 0x01));
    delay_us(8);
    nrf_gpio_pin_write(A800_LED_CLK,1);
    delay_us(8);

    // write bit 0
    nrf_gpio_pin_write(A800_LED_CLK,0);
    nrf_gpio_pin_write(A800_LED_DATA, ((data >> 0) & 0x01));
    delay_us(8);
    nrf_gpio_pin_write(A800_LED_CLK,1);
    delay_us(8);

    // output data to latches
    nrf_gpio_pin_write(A800_LED_ON,1);
    delay_us(8);
    nrf_gpio_pin_write(A800_LED_DATA,0);
    nrf_gpio_pin_write(A800_LED_CLK,0);
    nrf_gpio_pin_write(A800_LED_ON,0);

    nrf_gpio_cfg_default(A800_LED_DATA);
}

/** 
 * 关闭状态LED的灯光
 **/
void a800_led_off()
{
    write_led_data(0);
}

/** 
 * 启用状态LED的灯光
 **/
void a800_led_on()
{
    write_led_data(led_data);
    // if (!leds_turn_off_read()) {
    //     write_led_data(saved_a800_led_val);
    // }
}

/**
 * 设置所有状态灯的状态
 **/
void a800_led_all(bool state)
{
    led_data = state ? 0x1f : 0;
    write_led_data(led_data);
}

// /** 
//  * 通知所有灯光启用
//  **/
// static void set_led_on()
// {
//     a800_led_on();
//     power_save_reset();
// }

// /**
//  *  系统状态LED灯全开
//  * 
//  *  用于开关机时闪烁一次
//  */
// static void a800_led_all_on(void)
// {
//     write_led_data(0x0F);
// }

/**
 * @brief 开启闪烁灯
 * 
 */
static void a800_led_blink_on(uint8_t blink_mask_, uint32_t blink_mult_)
{
    if (blink_mask_ == 0) return;

    blink_mask |= blink_mask_;
    for (uint8_t i = 0; i < LED_CHANNEL_NUM; ++i)
    {
        if ((1 << i) & blink_mask_)
        {
            // 设置通道倍频值
            uint32_t mult = GET_BLINK_MULT(i, blink_mult_);
            blink_mult &= ~(MAKE_BLINK_MULT(i, 0xff));
            blink_mult |= MAKE_BLINK_MULT(i, mult);

            // 通道倍频计数器清零
            blink_mult_counting &= ~(MAKE_BLINK_MULT(i, 0xff));
        }
    }

    // if (blink_enable == false)
    // {
    //     app_timer_start(a800_led_blink_timer, APP_TIMER_TICKS(A800_LED_BLINK_INTERVAL), NULL);
    // }
    blink_enable = true;

    // 即刻更新闪烁状态
    a800_led_blink_led(false);
}

/**
 * @brief 关闭闪烁灯
 * 
 */
static void a800_led_blink_off(uint8_t blink_mask_)
{
    if (blink_mask_ == 0) return;

    blink_mask &= ~blink_mask_;
    
    if (blink_mask == 0)
    {
        blink_enable = false;
        // blink_enable = false后timer自动stop

        // 还原闪烁前led状态
        write_led_data(led_data);
    }
    else
    {
        // 即刻更新闪烁状态
        a800_led_blink_led(false);
    }
}

static void a800_led_set(bool state, uint8_t mask)
{
    led_data = (state ? led_data | mask : led_data & ~mask);
    write_led_data(led_data);
}

/** 
 * 设置CAP状态灯的状态
 **/
void a800_led_cap(bool state)
{
    // static uint8_t n = 0;

    // ++n;
    // n %= 4;

    // NRF_LOG_INFO( "a800_led_cap n=%d", n);

    // //if (++n & CAP_MASK)
    // {
    //     if (n & CAP_MASK) {
    //         a800_led_blink_on(G_MASK, MAKE_BLINK_MULT(G_CHANNEL, 2));

    //         NRF_LOG_INFO( "a800_led_cap G_MASK on");

    //     } else {
    //         a800_led_blink_off(G_MASK);
    //         NRF_LOG_INFO( "a800_led_cap G_MASK off");
    //     }
    // }

    // //if (n & NUM_MASK)
    // {
    //     if (n & NUM_MASK) {
    //         a800_led_blink_on(R_MASK, MAKE_BLINK_MULT(R_CHANNEL, 10));
    //         NRF_LOG_INFO( "a800_led_cap R_MASK on");
    //     } else {
    //         a800_led_blink_off(R_MASK);
    //         NRF_LOG_INFO( "a800_led_cap R_MASK off");
    //     }
    // }

    a800_led_set(state, CAP_MASK);
}

/** 
 * 设置NUM状态灯的状态
 **/
void a800_led_num(bool state)
{
    a800_led_set(state, NUM_MASK);
}

/** 
 * 设置RGB状态灯R分量状态
 **/
void a800_led_r(bool state)
{
    a800_led_set(state, R_MASK);
}

/** 
 * 设置RGB状态灯G分量状态
 **/
void a800_led_g(bool state)
{
    a800_led_set(state, G_MASK);
}

/** 
 * 设置RGB状态灯B分量状态
 **/
void a800_led_b(bool state)
{
    a800_led_set(state, B_MASK);
}

/** 
 * 设置蓝牙状态灯的状态
 **/
void a800_led_ble(bool state)
{
    a800_led_set(state, BLE_MASK);
}

/** 
 * 设置充电状态灯的状态
 **/
void a800_led_charging(bool state)
{
    a800_led_set(state, CHARGING_MASK);
}

/** 
 * 设置USB状态灯的状态
 **/
void a800_led_usb(bool state)
{
    a800_led_set(state, USB_MASK);
}

static void a800_led_evt_handler(enum user_event event, void* arg)
{
    uint8_t arg2 = (uint32_t)arg;
    switch (event) {
    case USER_EVT_STAGE:
        switch (arg2) {
        case KBD_STATE_POST_INIT: // 初始化LED
            app_timer_create(&a800_led_blink_timer, APP_TIMER_MODE_REPEATED, a800_led_blink_timer_handler);
            app_timer_start(a800_led_blink_timer, APP_TIMER_TICKS(A800_LED_BLINK_INTERVAL), NULL);
            break;
        case KBD_STATE_SLEEP: // 准备休眠
            a800_led_deinit();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_POWERSAVE:
        switch (arg2) {
        case PWR_SAVE_ENTER: // 进入省电模式
            a800_led_off();
            break;
        case PWR_SAVE_EXIT: // 退出省电模式
            a800_led_on();
            break;
        default:
            break;
        }
        break;
    case USER_EVT_CHARGE: // 充电事件
        a800_led_charging(arg2 == BATT_CHARGING);
        break;
    case USER_EVT_USB: // USB事件
        a800_led_usb(arg2 == USB_WORKING);
        break;
    case USER_EVT_BLE_DEVICE_SWITCH: // 蓝牙设备通道切换事件
        ble_device_id = arg2;
        break;
    case USER_EVT_BLE_STATE_CHANGE: // 蓝牙状态事件
        if (arg2 == BLE_STATE_FAST_ADV) {
            uint32_t mult = 1;
            if (ble_device_id ==1) mult = 4;
            if (ble_device_id ==2) mult = 8;

            a800_led_blink_on(BLE_MASK, MAKE_BLINK_MULT(BLE_CHANNEL, mult));
        } else {
            a800_led_blink_off(BLE_MASK);
        }
        // a800_led_ble(arg2 == BLE_STATE_CONNECTED);
        break;
    default:
        break;
    }
}

EVENT_HANDLER(a800_led_evt_handler);