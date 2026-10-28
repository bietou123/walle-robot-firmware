#ifndef SENSOR_H
#define SENSOR_H

#include "config.h"

float distLeft = 999;
float distRight = 999;
float distMin = 999;

void sensorInit() {
    pinMode(PIN_TRIG_L, OUTPUT);
    pinMode(PIN_ECHO_L, INPUT);
    pinMode(PIN_TRIG_R, OUTPUT);
    pinMode(PIN_ECHO_R, INPUT);
}

float readUltrasonic(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, ULTRASONIC_TIMEOUT_US);
    if (duration == 0) return 999.0;
    return duration * 0.0343 / 2.0;
}

void sensorUpdate() {
    distLeft = readUltrasonic(PIN_TRIG_L, PIN_ECHO_L);
    distRight = readUltrasonic(PIN_TRIG_R, PIN_ECHO_R);
    distMin = min(distLeft, distRight);
}

// 判断方向：人在左边还是右边
int sensorDirection() {
    float diff = distLeft - distRight;
    if (abs(diff) < 5) return 0;  // 正前方
    return (diff > 0) ? 1 : -1;   // 1=右边近, -1=左边近
}

#endif
