<!--
 * @Description: None
 * @Author: LILYGO_L
 * @Date: 2023-09-11 16:13:14
 * @LastEditTime: 2025-03-18 12:10:25
 * @License: GPL 3.0
-->
<h1 align = "center">T3-S3-MVSRBoard</h1>

## **English | [中文](./README_CN.md)**

## VersionIteration:
| Version                               | Update date                       |Update description|
| :-------------------------------: | :-------------------------------: |:--------------: |
| T3-S3-MVSRBoard_V1.0                      | 2024-11-06                    |   Original version      |
| T3-S3-MVSRBoard_V1.1                      | 2025-03-18                    |   Replace microphone model      |

## PurchaseLink
| Product                     | SOC           |  FLASH  |  PSRAM   | Link                   |
| :------------------------: | :-----------: |:-------: | :---------: | :------------------: |
| T3-S3-MVSRBoard_V1.0-V1.1   | NULL |   NULL   | NULL |  [NULL]()   |

## Directory
- [Describe](#describe)
- [Preview](#preview)
- [Module](#module)
- [SoftwareDeployment](#SoftwareDeployment)
- [PinOverview](#pinoverview)
- [RelatedTests](#RelatedTests)
- [FAQ](#faq)
- [Project](#project)

## Describe

The T3-S3-MVSRBoard is the backplate design for the T3-S3_V1.2 motherboard, featuring onboard speaker and microphone expansion with extremely low static current. Additionally, it includes vibration and RTC (Real-Time Clock) functions.

## Preview

### Actual Product Image

## Module

### 1. Speaker

* Chip: MAX98357A
* Bus communication protocol: IIS
* Other: Default using 9dB gain
* Related documentation: 
    >[MAX98357A](./information/MAX98357AETE+T.pdf)
* Dependent libraries: 
    >[Arduino_DriveBus-1.1.16](https://github.com/Xk-w/Arduino_DriveBus)

### 2. Microphone

> #### T3-S3-MVSRBoard_V1.0 version
> * Chip: MSM261S4030H0R
> * Bus communication protocol: IIS
> * Related documentation: 
>    >[MSM261S4030H0R](./information/MEMSensing-MSM261S4030H0R.pdf)
> * Dependent libraries: 
>     >[Arduino_DriveBus-1.1.16](https://github.com/Xk-w/Arduino_DriveBus)

> #### T3-S3-MVSRBoard_V1.1 version
> * Chip: MP34DT05-A
> * Bus communication protocol: PDM
> * Related documentation: 
>    >[MP34DT05-A](./information/mp34dt05-a.pdf)
> * Dependent libraries: 
>    >[Arduino_DriveBus-1.1.16](https://github.com/Xk-w/Arduino_DriveBus)

### 3. Vibration

* Bus communication protocol: PWM

### 4. RTC

* Chip: PCF85063ATL
* Bus communication protocol: IIC
* Related documentation: 
    >[PCF85063ATL](./information/PCF85063ATL-1,118.pdf)
* Dependent libraries: 
    >[Arduino_DriveBus-1.1.16](https://github.com/Xk-w/Arduino_DriveBus)

## SoftwareDeployment

### Examples Support

| Example | `[Platformio IDE][espressif32-v6.5.0]`<br />`[Arduino IDE][esp32_v2.0.14]` | Description | Picture |
| ------  | ------ | ------ | ------ | 
| [DMIC_ReadData](./examples/DMIC_ReadData) |  <p align="center">![alt text][supported] | | |
| [DMIC_SD](./examples/DMIC_SD) |  <p align="center">![alt text][supported] | | |
| [GFX](./examples/GFX) |  <p align="center">![alt text][supported] | | |
| [IIC_Scan_2](./examples/IIC_Scan_2) |  <p align="center">![alt text][supported] | | |
| [Original_Test](./examples/Original_Test) |  <p align="center">![alt text][supported] | Original factory program | |
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
| [Original_Test(SX1262)](./firmware/[T3-S3-MVSRBoard][Original_Test][SX1262]_firmware/) | Original factory program |  |
| [Original_Test(SX1276)](./firmware/[T3-S3-MVSRBoard][Original_Test][SX1276]_firmware/) | Original factory program |  |
| [Original_Test(SX1278)](./firmware/[T3-S3-MVSRBoard][Original_Test][SX1278]_firmware/) | Original factory program |  |
| [Original_Test(SX1280)](./firmware/[T3-S3-MVSRBoard][Original_Test][SX1280]_firmware/) | Original factory program |  |
| [Original_Test(SX1280PA)](./firmware/[T3-S3-MVSRBoard][Original_Test][SX1280PA]_firmware/) | Original factory program |  |

### PlatformIO
1. Install [VisualStudioCode](https://code.visualstudio.com/Download) ,Choose installation based on your system type.

2. Open the "Extension" section of the Visual Studio Code software sidebar(Alternatively, use "<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>X</kbd>" to open the extension),Search for the "PlatformIO IDE" extension and download it.

3. During the installation of the extension, you can go to GitHub to download the program. You can download the main branch by clicking on the "<> Code" with green text, or you can download the program versions from the "Releases" section in the sidebar.

4. After the installation of the extension is completed, open the Explorer in the sidebar(Alternatively, use "<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>E</kbd>" go open it),Click on "Open Folder," locate the project code you just downloaded (the entire folder), and click "Add." At this point, the project files will be added to your workspace.

5. Open the "platformio.ini" file in the project folder (PlatformIO will automatically open the "platformio.ini" file corresponding to the added folder). Under the "[platformio]" section, uncomment and select the example program you want to burn (it should start with "default_envs = xxx") Then click "<kbd>[√](image/4.png)</kbd>" in the bottom left corner to compile,If the compilation is correct, connect the microcontroller to the computer and click "<kbd>[→](image/5.png)</kbd>" in the bottom left corner to download the program.

### Arduino
1. Install [Arduino](https://www.arduino.cc/en/software) ,Choose installation based on your system type.

2. Open the "example" directory within the project folder, select the example project folder, and open the file ending with ".ino" to open the Arduino IDE project workspace.

3. Open the "Tools" menu at the top right -> Select "Board" -> "Board Manager." Find or search for "esp32" and download the board files from the author named "Espressif Systems." Then, go back to the "Board" menu and select the development board type under "ESP32 Arduino." The selected development board type should match the one specified in the "platformio.ini" file under the [env] section with the header "board = xxx." If there is no corresponding development board, you may need to manually add the development board from the "board" directory within your project folder.

4. Open menu bar "[File](image/6.png)" -> "[Preferences](image/6.png)" ,Find "[Sketchbook location](image/7.png)"  here,copy and paste all library files and folders from the "libraries" folder in the project directory into the "libraries" folder in this directory.

5. Select the correct settings in the Tools menu, as shown in the table below.

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
| Flash Size                           | 16MB (128Mb)                    |
| Core Debug Level                | None                                 |
| Partition Scheme                | 16M Flash (3MB APP/9.9MB FATFS) |
| PSRAM                                | OPI PSRAM                         |
| Arduino Runs On                  | Core 1                               |
| Events Run On                     | Core 1                               |           

6. Select the correct port.

7. Click "<kbd>[√](image/8.png)</kbd>" in the upper right corner to compile,If the compilation is correct, connect the microcontroller to the computer,Click "<kbd>[→](image/9.png)</kbd>" in the upper right corner to download.

### firmware download
1. Open the project file "tools" and locate the ESP32 burning tool. Open it.

2. Select the correct burning chip and burning method, then click "OK." As shown in the picture, follow steps 1->2->3->4->5 to burn the program. If the burning is not successful, press and hold the "BOOT-0" button and then download and burn again.

3. Burn the file in the root directory of the project file "[firmware](./firmware/)" file,There is a description of the firmware file version inside, just choose the appropriate version to download.

<p align="center" width="100%">
    <img src="image/10.png" alt="example">
    <img src="image/11.png" alt="example">
</p>


## PinOverview

| Speaker pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| BCLK         | IO40       |
| LRCLK         | IO41       |
| DATA         | IO39       |
| SD_MODE         | IO38       |

> #### T-Display-S3-Pro-MVSRBoard_V1.0 version
> | Microphone pins  | ESP32S3 pins|
> | :------------------: | :------------------:|
> | BCLK         | IO47       |
> | WS         | IO15       |
> | DATA         | IO48       |
> | EN         | IO35       |

> #### T-Display-S3-Pro-MVSRBoard_V1.1 version
> | Microphone pins  | ESP32S3 pins|
> | :------------------: | :------------------:|
> | LRCLK         | IO15       |
> | DATA         | IO48       |
> | EN         | IO35       |

| Vibration motor pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| DATA         | IO46       |

| RTC pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| SDA         | IO42       |
| SCL         | IO45       |
| INT         | IO16       |

| SX126x pins  | ESP32S3 pins|
| :------------------: | :------------------:|
| CS         | IO7       |
| RST         | IO8       |
| SCLK         | IO5       |
| MOSI         | IO6       |
| MISO         | IO3       |
| DIO1         | IO33       |
| BUSY         | IO34       |

| SX127x pins  | ESP32S3 pins|
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

| SX128x pins  | ESP32S3 pins|
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

## RelatedTests

### Power Dissipation
| Firmware | Program| Description | Picture |
| ------  | ------  | ------ | ------ | 
| [Sleep_Wake_Up](./firmware/[T3-S3-MVSRBoard][Sleep_Wake_Up][SX1262]_firmware_V1.0.0_202411041104.bin) | [Sleep_Wake_Up](./examples/Sleep_Wake_Up) | Static current: 2.77 μA for more information please refer to [Power Consumption Test Log](./relevant_test/PowerConsumptionTestLog_[T3-S3-MVSRBoard_V1.0]_20241104.pdf) | |

## FAQ

* Q. After reading the above tutorials, I still don't know how to build a programming environment. What should I do?
* A. If you still don't understand how to build an environment after reading the above tutorials, you can refer to the [LilyGo-Document](https://github.com/Xinyuan-LilyGO/LilyGo-Document) document instructions to build it.

<br />

* Q. Why does Arduino IDE prompt me to update library files when I open it? Should I update them or not?
* A. Choose not to update library files. Different versions of library files may not be mutually compatible, so it is not recommended to update library files.

<br />

* Q. Why is there no serial data output on the "Uart" interface on my board? Is it defective and unusable?
* A. The default project configuration uses the USB interface as Uart0 serial output for debugging purposes. The "Uart" interface is connected to Uart0, so it won't output any data without configuration.<br />For PlatformIO users, please open the project file "platformio.ini" and modify the option under "build_flags = xxx" from "-D ARDUINO_USB_CDC_ON_BOOT=true" to "-D ARDUINO_USB_CDC_ON_BOOT=false" to enable external "Uart" interface.<br />For Arduino users, open the "Tools" menu and select "USB CDC On Boot: Disabled" to enable the external "Uart" interface.

<br />

* Q. Why is my board continuously failing to download the program?
* A. Please hold down the "BOOT-0" button and try downloading the program again.

## Project
* [T3-S3-MVSRBoard_V1.0](./project/T3-S3-MVSRBoard_V1.0.pdf)

