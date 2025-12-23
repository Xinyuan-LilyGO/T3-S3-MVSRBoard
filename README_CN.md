<!--
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2023-09-11 16:13:14
 * @LastEditTime: 2025-12-23 09:55:48
 * @License: GPL 3.0
-->
<h1 align = "center">T3-S3-MVSRBoard</h1>

## **[English](./README.md) | 中文**

## 版本迭代:
| Version                               | Update date                       |Update description|
| :-------------------------------: | :-------------------------------: |:--------------: |
| T3-S3-MVSRBoard_V1.0                      | 2024-11-06                    |   初始版本      |
| T3-S3-MVSRBoard_V1.1                      | 2025-03-18                    |   更换麦克风型号      |

## 购买链接

| Product                     | SOC           |  FLASH  |  PSRAM   | Link                   |
| :------------------------: | :-----------: |:-------: | :---------: | :------------------: |
| T3-S3-MVSRBoard_V1.0-V1.1   | NULL |   NULL   | NULL |  [NULL]()   |

## 目录
- [描述](#描述)
- [预览](#预览)
- [模块](#模块)
- [软件部署](#软件部署)
- [引脚总览](#引脚总览)
- [相关测试](#相关测试)
- [常见问题](#常见问题)
- [项目](#项目)

## 描述

T3-S3-MVSRBoard为T3-S3_V1.2主板的背板设计，板载扬声器麦克风扩展，拥有极低的静态电流。除此之外还有振动和RTC功能。

## 预览

### 实物图

## 模块

### 1. 扬声器

* 芯片：MAX98357A
* 总线通信协议：IIS
* 其他：默认使用9dB增益
* 相关资料：
    >[MAX98357A](./information/MAX98357AETE+T.pdf)
* 依赖库：
    >[Arduino_DriveBus-1.1.16]()

### 2. 麦克风

> #### T3-S3-MVSRBoard_V1.0 版本
> * 芯片：MSM261S4030H0R
> * 总线通信协议：IIS
> * 相关资料：
>    >[MSM261S4030H0R](./information/MEMSensing-MSM261S4030H0R.pdf)
> * 依赖库：
>     >[Arduino_DriveBus-1.1.16]()

> #### T3-S3-MVSRBoard_V1.1 版本
> * 芯片：MP34DT05-A
> * 总线通信协议：PDM
> * 相关资料：
>    >[MP34DT05-A](./information/mp34dt05-a.pdf)
> * 依赖库：
>    >[Arduino_DriveBus-1.1.16]()

### 3. 振动

* 总线通信协议：PWM

### 4. RTC

* 芯片：PCF85063ATL
* 总线通信协议：IIC
* 相关资料：
    >[PCF85063ATL](./information/PCF85063ATL-1,118.pdf)
* 依赖库：
    >[Arduino_DriveBus-1.1.16]()

## 软件部署

### 示例支持

| Example | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` | Description | Picture |
| ------  | ------ | ------ | ------ | 
| [DMIC_ReadData](./examples/DMIC_ReadData) |  <p align="center">![alt text][supported] | | |
| [DMIC_SD](./examples/DMIC_SD) |  <p align="center">![alt text][supported] | | |
| [GFX](./examples/GFX) |  <p align="center">![alt text][supported] | | |
| [IIC_Scan_2](./examples/IIC_Scan_2) |  <p align="center">![alt text][supported] | | |
| [Original_Test](./examples/Original_Test) |  <p align="center">![alt text][supported] | 出厂程序 | |
| [PCF85063](./examples/PCF85063) |  <p align="center">![alt text][supported] | | |
| [PCF85063_Scheduled_INT](./examples/PCF85063_Scheduled_INT) |  <p align="center">![alt text][supported] | | |
| [PCF85063_Timer_INT](./examples/PCF85063_Timer_INT) |  <p align="center">![alt text][supported] | | |
| [SD](./examples/SD) |  <p align="center">![alt text][supported] | | |
| [SD_Music](./examples/SD_Music) |  <p align="center">![alt text][supported] | | |
| [Sleep_Wake_Up](./examples/Sleep_Wake_Up) |  <p align="center">![alt text][supported] | | |
| [SX126x_PingPong](./examples/SX126x_PingPong) |  <p align="center">![alt text][supported] | | |
| [SX126x_PingPong](./examples/SX126x_PingPong_2) |  <p align="center">![alt text][supported] | | |
| [SX126x_Walkie_Talkie](./examples/SX126x_Walkie_Talkie) |  <p align="center">![alt text][supported] | | |
| [SX127x_PingPong](./examples/SX127x_PingPong) |  <p align="center">![alt text][supported] | | |
| [SX127x_PingPong_2](./examples/SX127x_PingPong_2) |  <p align="center">![alt text][supported] | | |
| [SX128x_PingPong_2](./examples/SX128x_PingPong_2) |  <p align="center">![alt text][supported] | | |
| [Vibration_Motor](./examples/Vibration_Motor) |  <p align="center">![alt text][supported] | | |
| [Voice_Codec2_Speaker](./examples/Voice_Codec2_Speaker) |  <p align="center">![alt text][supported] | | |
| [Voice_Speaker](./examples/Voice_Speaker) |  <p align="center">![alt text][supported] | | |
| [Wifi_Music](./examples/Wifi_Music) |  <p align="center">![alt text][supported] | | |

[supported]: https://img.shields.io/badge/-supported-green "example"

| Firmware | Description | Picture |
| ------  | ------  | ------ |
| [Original_Test(V1.0)](./firmware/T3-S3-MVSRBoard_V1.0/) | 出厂程序 |  |
| [Original_Test(V1.1)](./firmware/T3-S3-MVSRBoard_V1.1/) | 出厂程序 |  |

### PlatformIO
1. 安装 [VisualStudioCode](https://code.visualstudio.com/Download) ，根据你的系统类型选择安装。

2. 打开VisualStudioCode软件侧边栏的“扩展”（或者使用<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>X</kbd>打开扩展），搜索“PlatformIO IDE”扩展并下载。

3. 在安装扩展的期间，你可以前往GitHub下载程序，你可以通过点击带绿色字样的“<> Code”下载主分支程序，也通过侧边栏下载“Releases”版本程序。

4. 扩展安装完成后，打开侧边栏的资源管理器（或者使用<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>E</kbd>打开），点击“打开文件夹”，找到刚刚你下载的项目代码（整个文件夹），点击“添加”，此时项目文件就添加到你的工作区了。

5. 打开项目文件中的“platformio.ini”（添加文件夹成功后PlatformIO会自动打开对应文件夹的“platformio.ini”）,在“[platformio]”目录下取消注释选择你需要烧录的示例程序（以“default_envs = xxx”为标头），然后点击左下角的“<kbd>[√](image/4.png)</kbd>”进行编译，如果编译无误，将单片机连接电脑，点击左下角“<kbd>[→](image/5.png)</kbd>”即可进行烧录。

### Arduino
1. 安装 [Arduino](https://www.arduino.cc/en/software) ，根据你的系统类型选择安装。

2. 打开项目文件夹的“example”目录，选择示例项目文件夹，打开以“.ino”结尾的文件即可打开Arduino IDE项目工作区。

3. 打开右上角“工具”菜单栏->选择“开发板”->“开发板管理器”，找到或者搜索“esp32”，下载作者名为“Espressif Systems”的开发板文件。接着返回“开发板”菜单栏，选择“ESP32 Arduino”开发板下的开发板类型，选择的开发板类型由“platformio.ini”文件中以[env]目录下的“board = xxx”标头为准，如果没有对应的开发板，则需要自己手动添加项目文件夹下“board”目录下的开发板。

4. 打开菜单栏“[文件](image/6.png)”->“[首选项](image/6.png)”，找到“[项目文件夹位置](image/7.png)”这一栏，将项目目录下的“libraries”文件夹里的所有库文件连带文件夹复制粘贴到这个目录下的“libraries”里边。

5. 在 "工具 "菜单中选择正确的设置，如下表所示。

#### ESP32-S3
| Setting                               | Value                                 |
| :-------------------------------: | :-------------------------------: |
| Board                                 | ESP32S3 Dev Module           |
| Upload Speed                     | 921600                               |
| USB Mode                           | Hardware CDC and JTAG     |
| USB CDC On Boot                | Enabled                              |
| USB Firmware MSC On Boot | Disabled                             |
| USB DFU On Boot                | Disabled                             |
| CPU Frequency                   | 240MHz (WiFi)                    |
| Flash Mode                         | QIO 80MHz                         |
| Flash Size                           | 4MB (32Mb)                    |
| Core Debug Level                | None                                 |
| Partition Scheme                | Huge APP (3MB No OTA/1MB SPIFFS) |
| PSRAM                                | Disabled                        |
| Arduino Runs On                  | Core 1                               |
| Events Run On                     | Core 1                               |        

6. 选择正确的端口。

7. 点击右上角“<kbd>[√](image/8.png)</kbd>”进行编译，如果编译无误，将单片机连接电脑，点击右上角“<kbd>[→](image/9.png)</kbd>”即可进行烧录。

### firmware烧录
1. 打开项目文件“tools”找到ESP32烧录工具，打开。

2. 选择正确的烧录芯片以及烧录方式点击“OK”，如图所示根据步骤1->2->3->4->5即可烧录程序，如果烧录不成功，请按住“BOOT-0”键再下载烧录。

3. 烧录文件在项目文件根目录“[firmware](./firmware/)”文件下，里面有对firmware文件版本的说明，选择合适的版本下载即可。

<p align="center" width="100%">
    <img src="image/10.png" alt="example">
    <img src="image/11.png" alt="example">
</p>

## 引脚总览

| 扬声器引脚  | ESP32S3引脚|
| :------------------: | :------------------:|
| BCLK         | IO40       |
| LRCLK         | IO41       |
| DATA         | IO39       |
| SD_MODE         | IO38       |

> #### T3-S3-MVSRBoard_V1.0 版本
> | 麦克风引脚  | ESP32S3引脚|
> | :------------------: | :------------------:|
> | BCLK         | IO47        |
> | WS         | IO15       |
> | DATA         | IO48        |
> | EN         | IO35       |

> #### T3-S3-MVSRBoard_V1.1 版本
> | 麦克风引脚  | ESP32S3引脚|
> | :------------------: | :------------------:|
> | LRCLK         | IO15       |
> | DATA         | IO48       |
> | EN         | IO35       |

| 振动马达引脚  | ESP32S3引脚|
| :------------------: | :------------------:|
| DATA         | IO46       |

| RTC引脚  | ESP32S3引脚|
| :------------------: | :------------------:|
| SDA         | IO42       |
| SCL         | IO45       |
| INT         | IO16       |

| SX126x引脚  | ESP32S3引脚|
| :------------------: | :------------------:|
| CS         | IO7       |
| RST         | IO8       |
| SCLK         | IO5       |
| MOSI         | IO6       |
| MISO         | IO3       |
| DIO1         | IO33       |
| BUSY         | IO34       |

| SX127x引脚  | ESP32S3引脚|
| :------------------: | :------------------:|
| CS         | IO7       |
| RST         | IO8       |
| SCLK         | IO5       |
| MOSI         | IO6       |
| MISO         | IO3       |
| DIO0         | IO9       |
| DIO1         | IO33       |
| DIO2         | IO34       |
| DIO3         | IO21       |
| DIO4         | IO10       |
| DIO5         | IO36       |

| SX128x引脚  | ESP32S3引脚|
| :------------------: | :------------------:|
| CS         | IO7       |
| RST         | IO8       |
| SCLK         | IO5       |
| MOSI         | IO6       |
| MISO         | IO3       |
| DIO1         | IO9       |
| BUSY         | IO36       |
| TX         | IO10       |
| RX         | IO21       |

## 相关测试

### 功耗
| Firmware | Program| Description | Picture |
| ------  | ------  | ------ | ------ | 
| [Sleep_Wake_Up](./firmware/[T3-S3-MVSRBoard_V1.0][Sleep_Wake_Up][SX1262]_firmware_V1.0.0_202411041104.bin) | [Sleep_Wake_Up](./examples/Sleep_Wake_Up) | 静态电流: 2.77 µA 更多信息请查看 [功耗测试日志](./relevant_test/PowerConsumptionTestLog_[T3-S3-MVSRBoard_V1.0]_20241104.pdf) | |

## 常见问题

* Q. 看了以上教程我还是不会搭建编程环境怎么办？
* A. 如果看了以上教程还不懂如何搭建环境的可以参考[LilyGo-Document](https://github.com/Xinyuan-LilyGO/LilyGo-Document)文档说明来搭建。

<br />

* Q. 为什么打开Arduino IDE时他会提醒我是否要升级库文件？我应该升级还是不升级？
* A. 选择不升级库文件，不同版本的库文件可能不会相互兼容所以不建议升级库文件。

<br />

* Q. 为什么我的板子上“Uart”接口没有输出串口数据，是不是坏了用不了啊？
* A. 因为项目文件默认配置将USB接口作为Uart0串口输出作为调试，“Uart”接口连接的是Uart0，不经配置自然是不会输出任何数据的。<br />PlatformIO用户请打开项目文件“platformio.ini”，将“build_flags = xxx”下的选项“-DARDUINO_USB_CDC_ON_BOOT=true”修改成“-DARDUINO_USB_CDC_ON_BOOT=false”即可正常使用外部“Uart”接口。<br />Arduino用户打开菜单“工具”栏，选择USB CDC On Boot: “Disabled”即可正常使用外部“Uart”接口。

<br />

* Q. 为什么我的板子一直烧录失败呢？
* A. 请按住“BOOT-0”按键重新下载程序。

<br />

* Q. 为什么我的MVSR板切换到发射模式时，对讲机示例会产生大声的嗡嗡声？
* A. T3-S3-MVSRBoard有两个版本，请确认自己的板子使用的是哪个版本，然后在[pin_config](/libraries/private_library/pin_config.h)中选择正确版本的宏定义重新烧录程序即可

## 项目
* [T3-S3-MVSRBoard_V1.0](./project/T3-S3-MVSRBoard_V1.0.pdf)
