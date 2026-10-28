#ifndef MOTORS_H
#define MOTORS_H

#include <ESP32Servo.h>
#include "config.h"

Servo headYaw;
Servo headPitch;
Servo armLeft;
Servo armRight;

void motorsInit() {
    headYaw.attach(PIN_HEAD_YAW, SERVO_MIN_US, SERVO_MAX_US);
    headPitch.attach(PIN_HEAD_PITCH, SERVO_MIN_US, SERVO_MAX_US);
    armLeft.attach(PIN_ARM_LEFT, SERVO_MIN_US, SERVO_MAX_US);
    armRight.attach(PIN_ARM_RIGHT, SERVO_MIN_US, SERVO_MAX_US);

    headYaw.write(HEAD_YAW_CENTER);
    headPitch.write(HEAD_PITCH_CENTER);
    armLeft.write(ARM_LEFT_DOWN);
    armRight.write(ARM_RIGHT_DOWN);

    // TB6612 pins
    pinMode(PIN_AIN1, OUTPUT);
    pinMode(PIN_AIN2, OUTPUT);
    pinMode(PIN_BIN1, OUTPUT);
    pinMode(PIN_BIN2, OUTPUT);
    pinMode(PIN_STBY, OUTPUT);
    ledcAttach(PIN_PWMA, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    ledcAttach(PIN_PWMB, MOTOR_PWM_FREQ, MOTOR_PWM_RES);

    digitalWrite(PIN_STBY, HIGH);
}

void trackStop() {
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, LOW);
    ledcWrite(PIN_PWMA, 0);
    ledcWrite(PIN_PWMB, 0);
}

void trackForward(uint8_t speed) {
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, LOW);
    ledcWrite(PIN_PWMA, speed);
    ledcWrite(PIN_PWMB, speed);
}

void trackBackward(uint8_t speed) {
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, HIGH);
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, HIGH);
    ledcWrite(PIN_PWMA, speed);
    ledcWrite(PIN_PWMB, speed);
}

void trackTurnLeft(uint8_t speed) {
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, HIGH);
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, LOW);
    ledcWrite(PIN_PWMA, speed);
    ledcWrite(PIN_PWMB, speed);
}

void trackTurnRight(uint8_t speed) {
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, HIGH);
    ledcWrite(PIN_PWMA, speed);
    ledcWrite(PIN_PWMB, speed);
}

// 舵机平滑移动
struct ServoMove {
    Servo* servo;
    int startAngle;
    int endAngle;
    unsigned long startTime;
    unsigned long duration;
};

void servoSmoothWrite(Servo& s, int targetAngle, int durationMs) {
    int current = s.read();
    int steps = abs(targetAngle - current);
    if (steps == 0) return;
    int delayPerStep = durationMs / steps;
    int dir = (targetAngle > current) ? 1 : -1;
    for (int i = 0; i < steps; i++) {
        s.write(current + dir * i);
        delay(delayPerStep);
    }
    s.write(targetAngle);
}

#endif
