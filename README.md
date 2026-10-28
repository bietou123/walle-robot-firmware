# Desktop Robot Firmware

ESP32-S3 桌面机器人固件原型，包含双路履带电机、四路舵机、超声波、ST7735 显示屏和本地网络控制逻辑。此仓库仅公开原创固件和接线草案，不包含第三方模型、品牌素材、参考教程、购买截图或可直接打印的成品文件。

## 使用

在 Arduino IDE 中选择 ESP32S3 Dev Module，安装 ESP32Servo、Adafruit GFX 和 Adafruit ST7735 库，打开 `firmware/walle-robot.ino`。接线草案见 `hardware/pcb/schematic_netlist.md`；实际硬件安装前请先校对电源、引脚及电机驱动。代码目前是实验性原型，不代表电路板或整机已通过验证。

Wi-Fi 和天气 API 配置应在自己的设备上设置，切勿将私人配置提交到仓库。此项目与任何影视角色的权利人无关联，第三方模型的使用与再分发需遵守其原始许可。

## 许可

仓库中原创源码按 MIT 许可证分发，见 `LICENSE`；未收录的第三方资源不在许可范围内。
