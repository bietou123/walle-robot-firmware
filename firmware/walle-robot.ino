/*
 * WALL-E Desktop Robot
 * ESP32-S3 + ILI9341 + TB6612 + HC-SR04 + SG90×4 + N20×2
 *
 * Board: ESP32S3 Dev Module
 * Partition: Default 4MB with spiffs (或 16MB 如果用 N16R8)
 * USB Mode: USB-OTG (CDC)
 */

#include "config.h"
#include "motors.h"
#include "sensor.h"
#include "display.h"
#include "behavior.h"
#include "network.h"

TaskHandle_t displayTaskHandle;
TaskHandle_t sensorTaskHandle;
TaskHandle_t behaviorTaskHandle;
TaskHandle_t networkTaskHandle;

void displayTaskFn(void* param) {
    displayInit();
    while (true) {
        displayRender();
        vTaskDelay(pdMS_TO_TICKS(33)); // ~30fps
    }
}

void sensorTaskFn(void* param) {
    sensorInit();
    while (true) {
        sensorUpdate();
        vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz 采样
    }
}

void behaviorTaskFn(void* param) {
    while (true) {
        behaviorUpdate();
        vTaskDelay(pdMS_TO_TICKS(50)); // 20Hz 决策
    }
}

void networkTaskFn(void* param) {
    loadWifiConfig();
    networkConnect();
    if (wifiConnected) {
        setupWebServer();
    }
    while (true) {
        if (wifiConnected) {
            server.handleClient();
            updateTime();
            updateWeather();
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== WALL-E Robot Starting ===");

    motorsInit();
    Serial.println("[OK] Motors");

    // 开机动画：头左右看看，手举一下
    delay(500);
    headYaw.write(60);
    delay(300);
    headYaw.write(120);
    delay(300);
    headYaw.write(HEAD_YAW_CENTER);
    delay(200);
    armLeft.write(ARM_LEFT_UP);
    armRight.write(ARM_RIGHT_UP);
    delay(500);
    armLeft.write(ARM_LEFT_DOWN);
    armRight.write(ARM_RIGHT_DOWN);

    // 启动 FreeRTOS 任务
    xTaskCreatePinnedToCore(displayTaskFn, "display", 8192, NULL, 2, &displayTaskHandle, 1);
    xTaskCreatePinnedToCore(sensorTaskFn, "sensor", 4096, NULL, 3, &sensorTaskHandle, 0);
    xTaskCreatePinnedToCore(behaviorTaskFn, "behavior", 4096, NULL, 2, &behaviorTaskHandle, 0);
    xTaskCreatePinnedToCore(networkTaskFn, "network", 8192, NULL, 1, &networkTaskHandle, 0);

    Serial.println("[OK] All tasks started");
    Serial.println("Web UI: http://" + WiFi.localIP().toString());
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
