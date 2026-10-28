# WALL-E Robot PCB — 原理图网表 + 嘉立创 BOM

## 原理图连接关系（完整网表）

### 电源部分
```
USB-C VBUS ──[自恢复保险丝 F1 2A]──> NET_5V
USB-C GND ──> GND
USB-C CC1 ──[R1 5.1kΩ]──> GND
USB-C CC2 ──[R2 5.1kΩ]──> GND
USB-C D+ ──> ESP32-S3 GPIO20 (USB_D+)
USB-C D- ──> ESP32-S3 GPIO19 (USB_D-)

NET_5V ──[C1 10μF]──> GND (输入滤波)
NET_5V ──> AMS1117-3.3 VIN
AMS1117-3.3 VOUT ──> NET_3V3
AMS1117-3.3 GND ──> GND
NET_3V3 ──[C2 10μF]──> GND (输出滤波)
NET_3V3 ──[C3 100nF]──> GND (高频去耦)
```

### ESP32-S3 模组
```
ESP32-S3-WROOM-1 引脚连接:
  VDD3V3 (Pin 2) ──> NET_3V3
  GND (Pin 1,40,41) ──> GND
  EN (Pin 3) ──[R3 10kΩ]──> NET_3V3  (上拉)
  EN (Pin 3) ──[C4 100nF]──> GND  (滤波)
  EN (Pin 3) ──[SW_RST]──> GND  (复位按键)
  GPIO0 (Pin 4) ──[R4 10kΩ]──> NET_3V3  (上拉)
  GPIO0 (Pin 4) ──[SW_BOOT]──> GND  (下载按键)
  GPIO19 (Pin 13) ──> USB-C D-
  GPIO20 (Pin 14) ──> USB-C D+

  GPIO10 ──> TFT_CS
  GPIO11 ──> TFT_DC
  GPIO12 ──> TFT_RST
  GPIO13 ──> TFT_SCK
  GPIO14 ──> TFT_MOSI
  GPIO9  ──> TFT_BLK

  GPIO1  ──> SERVO_YAW (J_SERVO1 信号)
  GPIO2  ──> SERVO_PITCH (J_SERVO2 信号)
  GPIO3  ──> SERVO_ARM_L (J_SERVO3 信号)
  GPIO4  ──> SERVO_ARM_R (J_SERVO4 信号)

  GPIO5  ──> TB6612 AIN1
  GPIO6  ──> TB6612 AIN2
  GPIO7  ──> TB6612 PWMA
  GPIO15 ──> TB6612 BIN1
  GPIO16 ──> TB6612 BIN2
  GPIO17 ──> TB6612 PWMB
  GPIO8  ──> TB6612 STBY

  GPIO41 ──> HC-SR04_L TRIG
  GPIO42 ──> HC-SR04_L ECHO
  GPIO45 ──> HC-SR04_R TRIG
  GPIO46 ──> HC-SR04_R ECHO

  GPIO38 ──> BUZZER (经三极管驱动)
  GPIO39 ──> WS2812 DIN (经100Ω串阻)
  GPIO48 ──> LED_STATUS (经1kΩ限流)
```

### TB6612FNG 电机驱动
```
TB6612FNG:
  VCC ──> NET_3V3 (逻辑供电)
  VM  ──> NET_5V  (电机供电)
  GND ──> GND
  AIN1 ──> GPIO5
  AIN2 ──> GPIO6
  PWMA ──> GPIO7
  BIN1 ──> GPIO15
  BIN2 ──> GPIO16
  PWMB ──> GPIO17
  STBY ──> GPIO8
  AO1 ──> J_MOTOR_L+ (左电机+)
  AO2 ──> J_MOTOR_L- (左电机-)
  BO1 ──> J_MOTOR_R+ (右电机+)
  BO2 ──> J_MOTOR_R- (右电机-)

  VM ──[C5 100nF]──> GND (电机电源去耦)
  VM ──[C6 10μF]──> GND
```

### 舵机接口 (4组, 每组3pin)
```
J_SERVO1: 信号(GPIO1) | VCC(NET_5V) | GND
J_SERVO2: 信号(GPIO2) | VCC(NET_5V) | GND
J_SERVO3: 信号(GPIO3) | VCC(NET_5V) | GND
J_SERVO4: 信号(GPIO4) | VCC(NET_5V) | GND
```

### 超声波接口 (2组, 每组4pin)
```
J_ULTRA_L: VCC(NET_5V) | TRIG(GPIO41) | ECHO(GPIO42) | GND
J_ULTRA_R: VCC(NET_5V) | TRIG(GPIO45) | ECHO(GPIO46) | GND
```

### 屏幕接口 (8pin排针) — 1.8" ST7735 (128×160)
```
J_TFT: GND | VCC(NET_3V3) | SCK(GPIO13) | MOSI(GPIO14) | RST(GPIO12) | DC(GPIO11) | CS(GPIO10) | BLK(GPIO9)
```

### 蜂鸣器驱动
```
GPIO38 ──[R5 1kΩ]──> SS8050 Base
SS8050 Emitter ──> GND
SS8050 Collector ──> BUZZER-
BUZZER+ ──> NET_5V
BUZZER两端并联 [D1 1N4148] (续流二极管, 阴极朝VCC)
```

### WS2812B
```
GPIO39 ──[R6 100Ω]──> WS2812 DIN
WS2812 VCC ──> NET_5V
WS2812 GND ──> GND
NET_5V ──[C7 100nF]──> GND (就近去耦)
```

### 状态LED
```
NET_3V3 ──[R7 1kΩ]──> LED_STATUS_Anode
LED_STATUS_Cathode ──> GPIO48 (低电平亮)
```

---

## BOM 清单 (LCSC 编号)

| # | 名称 | 规格 | 封装 | 数量 | LCSC | 单价(元) |
|---|------|------|------|------|------|----------|
| 1 | ESP32-S3-WROOM-1 | N16R8 | Module | 1 | C2913202 | 18.0 |
| 2 | USB-C 母座 | 16P 沉板 SMD | SMD | 1 | C2765186 | 1.2 |
| 3 | AMS1117-3.3 | 3.3V LDO | SOT-223 | 1 | C6186 | 0.4 |
| 4 | TB6612FNG | 双路H桥 | SSOP-24 | 1 | C90731 | 4.5 |
| 5 | SS8050 | NPN 三极管 | SOT-23 | 1 | C727942 | 0.1 |
| 6 | 1N4148 | 开关二极管 | SOD-123 | 1 | C81598 | 0.1 |
| 7 | 自恢复保险丝 | 2A | 1206 | 1 | C70069 | 0.3 |
| 8 | 电阻 5.1kΩ | 1% | 0805 | 2 | C27834 | 0.02 |
| 9 | 电阻 10kΩ | 1% | 0805 | 2 | C17414 | 0.02 |
| 10 | 电阻 1kΩ | 1% | 0805 | 2 | C17513 | 0.02 |
| 11 | 电阻 100Ω | 1% | 0805 | 1 | C17408 | 0.02 |
| 12 | 电容 10μF | 25V | 0805 | 3 | C15850 | 0.1 |
| 13 | 电容 100nF | 50V | 0805 | 5 | C49678 | 0.02 |
| 14 | LED 绿色 | 指示灯 | 0805 | 1 | C2297 | 0.05 |
| 15 | 按键开关 | 轻触 | 3×6mm SMD | 2 | C455280 | 0.2 |
| 16 | 排针 1×8 母 | 2.54mm直插 | DIP | 1 | C124413 | 0.5 |
| 17 | 排针 1×3 公 | 2.54mm直插 | DIP | 4 | C49257 | 0.1 |
| 18 | 排针 1×4 公 | 2.54mm直插 | DIP | 2 | C124378 | 0.1 |
| 19 | 排母 1×2 | 2.54mm电机 | DIP | 2 | C124412 | 0.1 |
| 20 | 无源蜂鸣器 | 5V | 9.6mm直插 | 1 | C96093 | 0.8 |

**PCB 板元件总成本: ~27元**

---

## PCB 布局规格

- **板子尺寸**: 55×40mm 矩形 (适配缩小后底盘)
- **层数**: 2层
- **厚度**: 1.6mm
- **铜厚**: 1oz
- **表面处理**: HASL(有铅) 最便宜
- **线宽/线距**: 最小 0.2mm/0.2mm
- **过孔**: 0.3mm/0.6mm (孔径/焊盘)

### 布局要点
```
┌─────────────────────────────────────┐
│  [USB-C]              [BOOT][RST]   │ ← 后边缘
│                                     │
│  [AMS1117]   [ESP32-S3-WROOM-1]     │
│  [C1][C2]         (模组)            │
│                                     │
│  [TB6612]    [模组继续...]           │
│  [C5][C6]                           │
│                                     │
│ [SERVO×4排针]  [超声波×2排针] [屏幕] │ ← 前边缘
└─────────────────────────────────────┘
   ↑左侧: MOTOR排针     右侧: LED/BUZ↑
```

- ESP32-S3 模组天线端 (PCB 天线那一侧) 朝板边缘外侧，周围不铺铜
- USB-C 焊盘加固（过孔锚定）
- GND 大面积铺铜（上下两层）
- 电机驱动区加粗走线 (0.5mm+)
- 所有去耦电容紧贴对应IC电源脚

---

## 嘉立创下单参数

```
数量: 5
层数: 2
尺寸: 55×40mm
板厚: 1.6mm
铜厚: 1oz
阻焊: 黑色 (配合瓦力工业风)
丝印: 白色
表面处理: 有铅喷锡
最小线宽: 6mil
最小孔径: 0.3mm
```

预估费用: **嘉立创2层板 5片 ≈ 5-15元** (常有优惠券)
