# 外置驱动

application/main/driver 目录下定义了一些设备的驱动。各个驱动的使用方法参见驱动目录内的README.md

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

### 注册一个新的通讯方式

```C
#include "keyboard_host_driver.h"

static struct host_driver custom_driver = {
    .keyboard_leds = &get_keyboard_led, // 获取LED值
    .queue_empty = &queue_empty, // 发送队列是否为空
    .send_packet = &custom_driver_send, // 发送数据包
    .driver_working = &custom_driver_working, // 当前驱动是否工作
};

// 以优先级3注册此驱动。
// 数字越小则优先级越高，会优先判断优先级高的驱动是否在工作并使用其通讯。
KEYBOARD_HOST_DRIVER(3, custom_driver);
```
