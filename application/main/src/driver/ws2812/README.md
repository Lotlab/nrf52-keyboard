## WS2812 RGB灯驱动

移植自QMK的WS2812驱动，修改适配NRF52芯片

### 启用配置
```
WS2812_ENABLE = yes

```

### Config 配置项目
```
#define WS2812_DI_PIN 8   //WS2812 RGB信号输入
#define WS2812_NUM 8   //WS2812灯数量
#define WS2812_ANIMATIONS  //开启RGB动态灯效
#define WS2812_PWR_PIN 12           //POWER控制针脚-采用P MOS
#define WS2812_PWR_PIN_REVERSE 12   //POWER控制针脚-采用N MOS

```
