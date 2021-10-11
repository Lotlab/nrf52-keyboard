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
#define SSD1306_LCDHEIGHT 64 //OLED屏幕的高度像素，当前支持32与64，默认为32
```
