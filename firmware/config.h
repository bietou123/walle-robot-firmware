#ifndef CONFIG_H
#define CONFIG_H

// ==================== 引脚定义 ====================

// 屏幕 (SPI - ST7735 1.8" 128×160)
#define TFT_CS    10
#define TFT_DC    11
#define TFT_RST   12
#define TFT_SCK   13
#define TFT_MOSI  14
#define TFT_BLK    9

// 舵机
#define PIN_HEAD_YAW    1
#define PIN_HEAD_PITCH  2
#define PIN_ARM_LEFT    3
#define PIN_ARM_RIGHT   4

// 履带电机 (TB6612FNG)
#define PIN_AIN1   5
#define PIN_AIN2   6
#define PIN_PWMA   7
#define PIN_BIN1  15
#define PIN_BIN2  16
#define PIN_PWMB  17
#define PIN_STBY   8

// 超声波
#define PIN_TRIG_L  41
#define PIN_ECHO_L  42
#define PIN_TRIG_R  45
#define PIN_ECHO_R  46

// 其他
#define PIN_BUZZER  38
#define PIN_LED_WS  39
#define PIN_STATUS  48

// ==================== 舵机参数 ====================

#define SERVO_FREQ      50
#define SERVO_MIN_US    500
#define SERVO_MAX_US    2400

#define HEAD_YAW_CENTER   90
#define HEAD_PITCH_CENTER 90
#define ARM_LEFT_DOWN     30
#define ARM_LEFT_UP      150
#define ARM_RIGHT_DOWN   150
#define ARM_RIGHT_UP      30

// ==================== 超声波参数 ====================

#define ULTRASONIC_TIMEOUT_US  30000
#define DIST_FAR     100  // cm, 检测范围上限
#define DIST_CURIOUS  80  // cm, 触发好奇
#define DIST_GREET    30  // cm, 触发打招呼
#define DIST_CLOSE    10  // cm, 触发兴奋

// ==================== 电机参数 ====================

#define MOTOR_PWM_FREQ   1000
#define MOTOR_PWM_RES    8
#define MOTOR_SPEED_LOW  100
#define MOTOR_SPEED_MED  180
#define MOTOR_SPEED_HIGH 255

// ==================== 网络 ====================

#define WIFI_CONNECT_TIMEOUT  10000
#define NTP_SERVER    "pool.ntp.org"
#define GMT_OFFSET    28800  // UTC+8 中国
#define WEATHER_API_KEY  ""  // OpenWeatherMap key, 通过Web页面配置
#define WEATHER_CITY     "Hangzhou"
#define WEATHER_INTERVAL 600000  // 10分钟更新一次

// ==================== 显示 ====================

#define SCREEN_W  128
#define SCREEN_H  160
#define SPI_SPEED 27000000

// ==================== 行为 ====================

#define IDLE_TIMEOUT     5000   // ms, 回到IDLE
#define BLINK_INTERVAL   3000   // ms, 眨眼间隔
#define MOOD_CHANGE_TIME 30000  // ms, 表情自动切换

#endif
