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

打开`main/project/arm5_no_packs/`目录下的keil工程，点击编译即可。

