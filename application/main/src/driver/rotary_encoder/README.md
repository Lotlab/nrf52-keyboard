## 旋转编码器驱动

### 启用配置
```
ROTARY_ENCODER = yes
```

### Config 配置项目
```
// 旋钮配置
#define ROTARY_ENCODER_A 19 // 编码器A脚IO
#define ROTARY_ENCODER_B 20 // 编码器B脚IO
// 旋钮正向按钮映射
#define ROTARY_ENCODER_POS 5,5 // 正向旋转映射到键盘的按键行列
// 旋钮负向按钮映射
#define ROTARY_ENCODER_NEG 5,6 // 负向旋转映射到键盘的按键行列

#define LOW_POWER_QDEC_TICKS 2 // 定义低功耗模式下旋转编码器每隔多少ms扫描一次，不写则不使用低功耗模式
```
