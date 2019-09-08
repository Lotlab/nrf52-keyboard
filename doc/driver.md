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

## 3灯LED状态灯事件响应

此驱动内使用了custom_event_handler事件钩子，实现了三个状态灯根据事件亮起的处理逻辑。
启用此驱动则会覆盖custom_event_handler，若不想覆盖则不要启用这个驱动

### 源文件
- 3led_status_evt.c 

### 启用配置
```
THREE_LED_STATUS_EVT = yes
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
