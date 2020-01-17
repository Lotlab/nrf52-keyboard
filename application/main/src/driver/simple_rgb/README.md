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
