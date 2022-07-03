## WS2812 RGB灯驱动

移植自QMK的WS2812驱动，修改适配NRF52芯片

### 源文件
- rgb.h

### 启用配置
```
RGBLIGHT_ENABLE = yes
```

### Config 配置项目
```
#define RGB_DI_PIN 8   //WS2812 RGB信号输入
#define RGBLED_NUM 8   //WS2812灯数量
#define RGBLIGHT_ANIMATIONS  //开启RGB动态灯效
#define RGB_PWR_PIN 12           //POWER控制针脚-采用P MOS
#define RGB_PWR_PIN_REVERSE 12   //POWER控制针脚-采用N MOS
```
