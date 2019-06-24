# nrf52-keyboard

## Overview

This is a TMK keyboard firmware for nRF52810, nRF51822 version see [here](https://github.com/Lotlab/nrf51822-keyboard).

## 概述

这是一个基于nrf52蓝牙键盘的固件，使用了nRF SDK 15.3作为底层硬件驱动，并使用TMK键盘库作为键盘功能的上部实现。

此固件中的USB部分和KeymapDownloader部分重用了 [nrf51822-keyboard](https://github.com/Lotlab/nrf51822-keyboard) 的代码。

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
- keyboard/ 各个键盘实现相关
- SDK/ nRF52 SDK
- tmk/ tmk core 相关
- usb/ USB部分代码

## 功能亮点

- 蓝牙/USB双模切换
- USB全键无冲
- 配列下载更新
- 电量上传
- 支持多媒体按键
- 耗电量低至400ua（使用lot60-ble硬件在关闭所有灯光条件下测得，不代表所有条件下的状态）
- 高度自定义的事件系统

## 硬件支持

当前支持nrf52810和nrf52832两种主控硬件，此固件支持的键盘列表见Keyboard目录。

## 编译

首先下载 [nRF5 SDK 15.3](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs), 解压并放入SDK文件夹。
然后安装 gcc-arm-none-eabi-7-2018-q2-update，将template目录中对应平台的配置文件模板复制一份，重命名为`Makefile.posix`或`Makefile.windows`，修改里面工具路径为你的安装目录。

### Bootloader 的编译
参见[这篇文章](https://devzone.nordicsemi.com/b/blog/posts/getting-started-with-nordics-secure-dfu-bootloader)，先编译uECC库，然后再编译Bootloader

```
cd application/bootloader/project/armgcc
make
```
### 蓝牙程序和USB控制器的编译
现在蓝牙和USB控制器程序的Makefile都放在一起了。进入对应的硬件目录，直接make即可。

```
cd keyboard/lot60-ble
make
```
## 硬件移植
若使用的硬件方案与已有的硬件相同，则可以将keyboard文件夹内的配置文件复制一份并修改。主要修改项目是Makefile与config.h。

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
       +------ 当前Protocol
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
0b1aab bbbb 
   ||+ ++++--- 数据包的数据部分长度
   ++--------- 数据包类型: 0: keyboard, 1: consumer, 2: system, 3: nkro keyboard
```
DAT长度由上面定义

#### Keymap 响应
同 Ping 包与当前状态的格式。0x11为成功，0x10为失败。
