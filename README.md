# nrf52810-keyboard

## Overview

This is a TMK keyboard firmware for nRF52810, nRF51822 version see [here](https://github.com/Lotlab/nrf51822-keyboard).

## 概述

这是一个基于nrf52810蓝牙键盘的固件，使用了nRF SDK 10.0作为底层硬件驱动，并使用TMK键盘库作为键盘功能的上部实现。

## 软件

目录说明：

- bootloader：用于DFU更新固件的Bootloader
- main：主程序
- sdk：nRF SDK
- tmk_core：TMK源码
- usb：双模的USB部分
- [KeymapDownloader](https://github.com/Lotlab/nrf51822-keyboard/tree/master/KeymapDownloader)：配列下载器

## 硬件

这个固件是为[BLE60-keyboard](https://github.com/Lotlab/BLE60-keyboard)设计的，请切换至type-c分支查看最新的硬件信息

## 编译

```
cd application/main/project/armgcc
make flash
```

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
- [x] 自定义各种灯光（？）
- [x] UART 通讯

## 默认指示灯颜色表

- <span style="color: #FFFFFF">白色</span>：无连接
- <span style="color: #66CCFF">天依蓝</span>：蓝牙已连接
- <span style="color: #0099FF">天蓝色</span>：USB已连接
- <span style="color: #FF8000">橙色</span>：充电中
- <span style="color: #00FF00">绿色</span>：充电完毕
- <span style="color: #FFFF00">黄色</span>：输入配对密码
- <span style="color: #FF0080">紫红色</span>：配对密码输入完毕
- <span style="color: #FF00FF">紫色</span>：休眠
