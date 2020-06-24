## 可配置 LED 状态灯

此驱动定义了一个基于事件的通用可配置 LED 状态指示灯。

### 启用配置
```
LEDMAP = yes
```

### config配置项目

```C

// 定义三个LED灯，7, 8 两个灯拉低亮，9 灯拉高亮
static const struct ledmap_led ledmap_leds[] = {
    { .dir = 0, .pin = 7 }, 
    { .dir = 0, .pin = 8 }, 
    { .dir = 1, .pin = 9 }
};

// 定义事件
static const struct ledmap_event ledmap_events[] = {
    {
        // 启动后亮起所有灯
        .event = USER_EVT_STAGE,
        .param = KBD_STATE_POST_INIT,
        .led_mask = 0xFF,
        .action = { .priority = 3, .action = TRIG_LED_ONESHOT(2) },
    },
    {
        // 蓝牙连接后亮起灯1
        .event = USER_EVT_BLE_STATE_CHANGE,
        .param = BLE_STATE_CONNECTED,
        .led_mask = 0x01,
        .action = { .priority = 0, .action = TRIG_LED_ON },
    },
    {
        // 蓝牙断开后关闭灯1
        .event = USER_EVT_BLE_STATE_CHANGE,
        .param = BLE_STATE_IDLE,
        .led_mask = 0x01,
        .action = { .priority = 0, .action = TRIG_NO_ACTION },
    },
    {
        // USB连接后亮起灯2
        .event = USER_EVT_USB,
        .param = USB_WORKING,
        .led_mask = 0x02,
        .action = { .priority = 1, .action = TRIG_LED_ON },
    },
    {
        .event = USER_EVT_USB,
        .param = USB_NOT_CONNECT,
        .led_mask = 0x02,
        .action = { .priority = 1, .action = TRIG_NO_ACTION },
    },
    {
        .event = USER_EVT_USB,
        .param = USB_NO_HOST,
        .led_mask = 0x02,
        .action = { .priority = 1, .action = TRIG_NO_ACTION },
    },
    {
        .event = USER_EVT_USB,
        .param = USB_NOT_WORKING,
        .led_mask = 0x02,
        .action = { .priority = 1, .action = TRIG_LED_OFF },
    },
    {
        // 未充电时关闭灯3
        .event = USER_EVT_CHARGE,
        .param = BATT_NOT_CHARGING,
        .led_mask = 0x04,
        .action = { .priority = 0, .action = TRIG_NO_ACTION },
    },
    {
        // 充电时闪烁灯3
        .event = USER_EVT_CHARGE,
        .param = BATT_CHARGING,
        .led_mask = 0x04,
        .action = { .priority = 0, .action = TRIG_LED_BLINK_FOREVER(3) },
    },
    {
        // 充满后亮起灯3
        .event = USER_EVT_CHARGE,
        .param = BATT_CHARGED,
        .led_mask = 0x04,
        .action = { .priority = 0, .action = TRIG_LED_ON },
    },
};


```

### 设计思想

此模块设计就是提供一个易于配置的灯光系统。此系统在事件触发后会执行相应的灯光操作，给配置带来便利。

各动作的内容见 ledmap.h，事件相关内容见events.h

- 优先级：每个事件对应的灯光都有一个优先级0-3。优先级高的灯光会覆盖优先级低的灯光。使用 TRIG_NO_ACTION 可以使此优先级灯光停止覆盖低优先级的灯光。
- 灯光类型：
  - 常亮：包括开启、关闭、反转
  - 闪烁：拥有两个参数，闪烁次数和时长，取值范围0-3。若次数设置为0则一直闪烁。

