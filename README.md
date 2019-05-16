# nrf52810-keyboard

## Overview

This is a TMK keyboard firmware for nRF52810, nRF51822 version see [here](https://github.com/Lotlab/nrf51822-keyboard).

## 概述

这是一个基于nrf52810蓝牙键盘的固件，使用了nRF SDK 15.3作为底层硬件驱动，并使用TMK键盘库作为键盘功能的上部实现。

此固件中的USB部分和KeymapDownloader部分重用了 [nrf51822-keyboard](https://github.com/Lotlab/nrf51822-keyboard) 的代码。

## 软件

目录说明：

- bootloader：用于DFU更新固件的Bootloader
- main：主程序
- sdk：nRF SDK
- tmk_core：TMK源码
- usb：双模的USB部分
- KeymapDownloader：配列下载器

## 硬件

这个固件是为[BLE60-keyboard](https://github.com/Lotlab/BLE60-keyboard)设计的，请切换至type-c分支查看最新的硬件信息

## 编译

首先下载 [nRF5 SDK 15.3](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs), 解压并放入SDK文件夹。
然后安装 gcc-arm-none-eabi-7-2018-q2-update，将template目录中对应平台的配置文件模板复制一份，重命名为`Makefile.posix`或`Makefile.windows`，修改里面工具路径为你的安装目录。

### Bootloader 的编译
参见[这篇文章](https://devzone.nordicsemi.com/b/blog/posts/getting-started-with-nordics-secure-dfu-bootloader)，先编译uECC库，然后再编译Bootloader

```
cd application/bootloader/project/armgcc
make
```
### 主程序的编译
```
cd application/main/project
make
```
### USB控制器的编译
安装CodeBlocks与GCC，打开工程编译即可。

## 目录结构
- application/ 固件相关
  - main/ 主程序
    - src/ 源码
      - ble/ 蓝牙相关代码
      - tmk/ tmk桥接相关
      - config/ 硬件配置相关
    - project/ 工程
  - bootloader/ 
    - src/ 源码
    - project/ 工程
- SDK/ nRF52 SDK
- tmk/ tmk core 相关
- usb/ USB部分代码

## Todo

- [x] EEConfig 配置存储
- [x] Bootloader Jump 跳转至Bootloader
- [x] Keymap 存储
- [x] 关机原因存储并快速恢复
- [ ] 多设备切换（？）
- [x] 自定义各种灯光
- [x] UART 通讯
- [ ] USB 主控程序使用 Make 编译

## 默认指示灯颜色表

- <span style="color: #FFFFFF">白色</span>：无连接
- <span style="color: #66CCFF">天依蓝</span>：蓝牙已连接
- <span style="color: #0099FF">天蓝色</span>：USB已连接
- <span style="color: #FF8000">橙色</span>：充电中
- <span style="color: #00FF00">绿色</span>：充电完毕
- <span style="color: #FFFF00">黄色</span>：输入配对密码
- <span style="color: #FF0080">紫红色</span>：配对密码输入完毕
- <span style="color: #FF00FF">紫色</span>：休眠

## UART 通讯协议

### 基础格式

CMD DAT ... DAT SUM

- CMD：命令
- DAT：数据
- SUM：前面所有数据和命令的校验和

其中，根据CMD的不同，DAT的长度可能有所变化。若DAT长度为0，则不需要SUM。

主机(CH554)会定期向从机(nRF52810)发送状态数据包，请求从机上传。

### 主机命令

#### Ping 包与当前状态
```
0b0001 xxxx
       ||||
       |||+--- 上次接收数据是否成功（成功置为1）
       ||+---- 充电状态（充满置为1）
       |+----- 主机状态（与主机连接成功置为1）
       +------ 预留
```

无DAT

#### LED 下传
```
0b010x xxxx
     + ++++--- 5Bit的LED状态
```

无DAT

#### Keymap 下传
```
0b1xxx xxxx 
   ||| ||||
   +++-++++--- 当前Keymap分包的ID，从0开始
```

DAT长度为60，存储着Keymap数据

### 从机命令

#### 按键数据包上传
```
0b10aa bbbb 
    || ++++--- 数据包的数据部分长度
    ++-------- 数据包类型: 0: keyboard, 1: consumer, 2: system
```
DAT长度由上面定义

#### Keymap 响应
同 [Ping 包与当前状态](#Ping 包与当前状态)
