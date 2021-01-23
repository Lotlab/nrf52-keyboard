# nrf52-keyboard
![Build Status](https://api.travis-ci.org/Lotlab/nrf52-keyboard.svg?branch=master)

## Overview

This is a TMK based keyboard firmware for nRF52 series, now support both nRF52810 and nRF52832. Firmware for nRF51822 see [here](https://github.com/Lotlab/nrf51822-keyboard).

## Directory Structure

- application: contians the bootloader and application for nrf52
  - main: main program for nrf52
    - src: source
      - ble: bluetooth related code 
      - config: sdk config
      - driver: driver for peripheral device
      - keyboard: keyboard logic 
      - protocol: communication protocol
      - tmk: tmk adaptor
    - project
  - bootloader: bootloader for nrf52
    - project
- doc: documents
- keyboard: keyboard config 
- SDK: directory for placing nrf SDK
- template: nrf SDK makefile template
- tmk: tmk source code
- usb: code for ch554

## Code Branch

- Master: is not stable, may contains some critical bug.
- Develop: is not stable, including some unfinished feature, may not pass compile.

If you want to use this project in your major keyboard, please see the Release page.

## Highlights

- BLE/USB dual mode
- USB NKRO
- Macro support
- Dynamic keymap/macro configuration
- Battery level upload
- Mousekey and media key support
- Low power comsumption. (~200ua when all LED off in Lot60-BLE)
- Support rotary encoder and other peripheral device (see driver directory)
- Highly customable event system

## Hardware supporting

Currently we support both nRF52810 and nRF52832. See keyboard directory for more information.

## Compile

Firstly, you should download [nRF5 SDK 15.3](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs), decompress it and put it into the SDK folder. The structure of SDK folder will be `SDK/components`, ...

And then, install `gcc-arm-none-eabi-7-2018-q2-update`. Copy the `Makefile.posix.template` or `Makefile.windows.template` to `Makefile.posix` or `Makefile.windows` (depending your OS), then modify the toolchain path in the file to your gcc installed path.

Install [SDCC](http://sdcc.sourceforge.net/) to compile code for CH554. Note: This project require SDCC 4.0 to compile, older version in ubuntu 18.04 and debian 10 will not work.

If you want to compile the bootloader, you should firstly complie the uECC library. See this [article](https://devzone.nordicsemi.com/b/blog/posts/getting-started-with-nordics-secure-dfu-bootloader).

Then, 
```bash
cd keyboard/lot60-ble
make # Compile main program and the USB program
make bootloader # Compile bootloader
```

We also provide Docker image for you to compile, this image contains toolchain and sdk. 
```bash
docker run lotlab/nrf52-keyboard
```

## Flashing

It's recommend to use DAP-Link to flashing nrf52 chip. If you want to do so, please install [pyocd](https://github.com/mbedmicro/pyOCD). If you want to generate DFU package, you should install [nrfutil](https://github.com/NordicSemiconductor/pc-nrfutil/).

For ch554, you could use the official [flasing utility](http://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) in windows, or third-party [usbisp](https://github.com/rgwan/librech551) in linux.

Type `make help` for all flashing command.

## 概述

这是一个基于nrf52蓝牙键盘的固件，使用了nRF SDK 15.3作为底层硬件驱动，并使用TMK键盘库作为键盘功能的上部实现。

## 目录结构
- application/ 固件相关
  - main/ 主程序
    - src/ 源码
      - ble/ 蓝牙相关代码
      - config/ 硬件配置相关
      - protocol/ 通讯相关
      - driver/ 外置驱动
      - tmk/ tmk桥接相关
    - project/ 工程
  - bootloader/ 
    - src/ 源码
    - project/ 工程
- keyboard/ 各个键盘实现相关
- SDK/ nRF52 SDK
- tmk/ tmk core 相关
- usb/ USB部分代码

## 分支说明

- Master 分支是不稳定的分支，可能存在一些BUG
- Develop 分支是更不稳定的分支，可能会出现无法编译通过的问题

若想要日常使用，建议使用Release版本

## 功能亮点

- 蓝牙/USB双模切换
- USB全键无冲
- 配列下载更新
- 电量上传
- 支持多媒体按键和鼠标键
- 支持按键宏
- 耗电量低至200ua（使用lot60-ble硬件在关闭所有灯光条件下测得，不代表所有条件下的状态）
- 高度自定义的事件系统
- 支持旋钮、WS2812等外设（详见drivers目录）

## 硬件支持

当前支持nrf52810和nrf52832两种主控硬件，此固件支持的键盘列表见Keyboard目录。

## 编译

首先下载 [nRF5 SDK 15.3](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs), 解压并放入SDK文件夹。
然后安装 gcc-arm-none-eabi-7-2018-q2-update，将template目录中对应平台的配置文件模板复制一份，重命名为`Makefile.posix`或`Makefile.windows`，修改里面工具路径为你的安装目录。

然后安装 [SDCC](http://sdcc.sourceforge.net/) 用于编译CH554相关代码。注意：需要 SDCC 4.0 及以上版本才可编译，部分操作系统（如ubuntu 18.04, debian 10）内的 SDCC 版本无法满足要求。

我们也提供了Docker镜像方便编译，请使用
```bash
docker run lotlab/nrf52-keyboard
```

### Bootloader 的编译
参见[这篇文章](https://devzone.nordicsemi.com/nordic/short-range-guides/b/software-development-kit/posts/getting-started-with-nordics-secure-dfu-bootloader)，先编译uECC库，然后再编译Bootloader

### 蓝牙程序和USB控制器的编译
现在蓝牙和USB控制器程序的Makefile都放在一起了。进入对应的硬件目录，直接make即可。

```bash
cd keyboard/lot60-ble
make # 编译主程序和USB控制程序
make bootloader # 编译bootloader
```

### 硬件的烧录

对于nrf52，若要通过JLink直接写入，则需要安装JLink的驱动；若使用DAP-Link写入，则需要安装[pyocd](https://github.com/mbedmicro/pyOCD)；若使用蓝牙DFU进行升级，则需要安装[nrfutil](https://github.com/NordicSemiconductor/pc-nrfutil/)

对于ch554，你可以使用官方的[windows烧写工具](http://www.wch.cn/downloads/WCHISPTool_Setup_exe.html)，或三方的[usbisp](https://github.com/rgwan/librech551)烧写。

请使用`make help`查看所有的烧写和打包指令。

### 详细教程

如果对上面的固件编译流程有问题，可参考Lotlab Wiki上的[这篇文章](https://wiki.lotlab.org/page/bluetooth/build-guide/)，或查看`.travis.yml`作为参考。

## 硬件移植
请参考Keyboard目录下的template移植模板，并查看doc目录下的对应说明。

## 捐助

如果你觉得这个工程有帮助到你，为何不请我喝一杯奶茶呢？

![支付宝二维码 donate@lotlab.org](https://raw.githubusercontent.com/Lotlab/mcsgyz/gh-pages/pic/alipay.jpg)
