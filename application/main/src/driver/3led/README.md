## 3灯LED状态灯驱动
此驱动定义了基本的3个或2个状态灯，包括蓝牙连接状态灯、USB工作状态灯、充电状态灯以及广播时闪烁对应通道的指示灯。

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
#define LED_STATUS_USB 23
#define LED_STATUS_CHARGING 24 
#define LED_BLE_CHANNEL1 BIT_LED_BLE # 指定 channel 映射
#define LED_BLE_CHANNEL2 BIT_LED_USB
#define LED_BLE_CHANNEL3 BIT_LED_CHARGING
```
### 配置说明
```
当时配置3个LED指示灯时，一般为蓝牙连接状态、USB工作状态、充电状态。
也可以仅配置2个指示灯，一般为蓝牙连接状态、USB工作状态，当仅使用2个指示灯时，会同时闪烁2个指示灯来指示蓝牙设备第三通道的广播状态
```
