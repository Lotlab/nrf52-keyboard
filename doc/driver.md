# 驱动编写说明

现有的驱动系统是基于事件系统搭建而成的，其主要思路是通过注册一个事件处理函数，来处理各个事件。

## 快速入门

假设要编写一个custom_driver.c：
``` C
// 引入事件系统头文件
#include "../keyboard/keyboard_evt.h"

// 编写事件处理函数
static void custom_driver_event_handler(enum user_event event, void* arg)
{
    // code...
}

// 注册事件
EVENT_HANDLER(custom_driver_event_handler);
```

然后在 driver.mk 内添加编译控制命令即可。

## 事件说明

详细见events.h。
默认的所有事件的arg都**不是**一个指针，而是一个枚举。需要使用的时候请将这个参数强制转换为枚举。

若需要自定义事件，请自行在驱动模块内定义事件的event（ID）和参数类型。

## 其他模块化配置

### 自定义FN处理过程

``` C
#include "../keyboard/keyboard_fn.h"

void custom_fn_handler(keyrecord_t* record, uint8_t id, uint8_t opt) {
    // TMK Fn正常处理流程
}

FN_HANDLER(custom_fn_handler);
```

### 添加额外的ADC测量通道

``` C
#include "../keyboard/adc_convert.h"

static struct adc_channel_config custom_adc_channel = {
    .adc_start = &adc_on_start, // 测量开始事件
    .adc_finish = &adc_result_handler, // 测量结果handler
    .period = 2000, // 测量周期，ms
    .config = &channel_config, // 通道配置
};

ADC_CONVERT_CHANNEL(custom_adc_channel);
```

### 在配置存储中注册一个条目

``` C
#include "../keyboard/data_storage.h"

// 注册一个名为item的，长度为4的存储区
CONFIG_SECTION(item, 4);

void write_item() {
    // 数据操作
    memcpy(item.data, src, item.len);
    // 写入存储区
    storage_write(1 << STORAGE_CONFIG);
}
```

### 注册一个自定义的HID配置项目

``` C
#include "../protocol/hid_configuration.h"
#include "../keyboard/data_storage.h"

CONFIG_SECTION(item, 4); // 注册存储区
HID_CONFIG(0x20, item); // 注册配置项目，ID为0x20
```

# 自定义驱动

application/main/driver 目录下定义了一些设备的驱动。

若要在你的键盘中使用这些驱动，仅需在你的Makefile内加入对应驱动的启用配置即可。

## 3灯LED状态灯驱动
此驱动定义了基本的三个状态灯，包括蓝牙连接状态灯，充电状态灯以及USB工作状态灯。

### 源文件
- 3led_status.c 
- 3led_status.h

### 启用配置
```
THREE_LED_STATUS = yes
```
### config配置项目
```
#define LED_STATUS_BLE 22
#define LED_STATUS_CHARGING 23
#define LED_STATUS_USB 24
```

## 单 RGB LED 灯驱动

使用PWM驱动普通的三个引脚的 RGB LED，可以用作状态指示灯。

此驱动没有内置事件实现。

### 源文件
- led_rgb.c
- led_rgb.h

### 启用配置
```
ONE_RGB_LED = yes
```

### Config配置项目
```
#define LED_RGB_CC // 是否为共阴LED
#define LED_RGB_R 22 // 红色引脚 
#define LED_RGB_G 23 // 绿色引脚
#define LED_RGB_B 24 // 蓝色引脚
```

## SSD1306 128*32 OLED 屏幕驱动

一个128*32的OLED屏幕的驱动，用于显示当前的状态等信息

### 启用配置
```
SSD1306_OLED = yes
```

### Config 配置项目
```
#define SSD1306_SDA 9 // OLED屏幕的I2C SDA引脚编号
#define SSD1306_SCL 10 // OLED屏幕的I2C SCL引脚编号
#define SSD1306_ROTATE_180 // 旋转180度
```

## 旋转编码器驱动

### 启用配置
```
ROTARY_ENCODER = yes
```

### Config 配置项目
```
// 旋钮配置
#define ROTARY_ENCODER_A 19 // 编码器A脚IO
#define ROTARY_ENCODER_B 20 // 编码器B脚IO
// 旋钮正向按钮映射
#define ROTARY_ENCODER_POS 5,5 // 正向旋转映射到键盘的按键行列
// 旋钮负向按钮映射
#define ROTARY_ENCODER_NEG 5,6 // 负向旋转映射到键盘的按键行列
```