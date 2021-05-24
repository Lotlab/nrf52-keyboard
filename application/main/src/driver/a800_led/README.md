## Benq A800 LED状态灯驱动

Benq A800三个状态灯由带锁存功能的串进并出74HC595进行驱动. 其中一个RGB灯用于蓝牙, USB, 充电状态指示; 另外两个普通绿色LED用于大小写锁定和数字键盘指示.

### 源文件
- a800_led.c 
- a800_led.h

### 启用配置
```
A800_LED_ENABLE = yes
```
### config配置项目
```
#define A800_LED_DATA 5    // LED数据pin定义(74HC595 DS)
#define A800_LED_CLK 3     // LED时钟pin定义(74HC595 SHCP)
#define A800_LED_ON 4      // LED使能pin定义(74HC595 STCP)
```

