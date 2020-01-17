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
