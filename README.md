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
- [ ] Keymap 存储
- [ ] 关机原因存储并快速恢复
- [ ] 多设备切换（？）
- [ ] 自定义各种灯光（？）
- [ ] SPI通讯