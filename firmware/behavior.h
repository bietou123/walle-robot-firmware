#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "config.h"
#include "motors.h"
#include "sensor.h"

enum State {
    STATE_IDLE,
    STATE_CURIOUS,
    STATE_GREETING,
    STATE_EXCITED,
    STATE_SAD,
    STATE_SLEEP
};

State currentState = STATE_IDLE;
State prevState = STATE_IDLE;
unsigned long stateEnterTime = 0;
unsigned long lastDetectTime = 0;

void enterState(State newState) {
    if (newState == currentState) return;
    prevState = currentState;
    currentState = newState;
    stateEnterTime = millis();
}

// 返回当前状态应显示的表情ID
int getExpression() {
    switch (currentState) {
        case STATE_IDLE:     return 0;
        case STATE_CURIOUS:  return 1;
        case STATE_GREETING: return 2;
        case STATE_EXCITED:  return 3;
        case STATE_SAD:      return 4;
        case STATE_SLEEP:    return 5;
        default:             return 0;
    }
}

void behaviorUpdate() {
    unsigned long now = millis();
    unsigned long elapsed = now - stateEnterTime;

    switch (currentState) {
        case STATE_IDLE:
            headYaw.write(HEAD_YAW_CENTER);
            headPitch.write(HEAD_PITCH_CENTER);
            armLeft.write(ARM_LEFT_DOWN);
            armRight.write(ARM_RIGHT_DOWN);
            trackStop();

            if (distMin < DIST_CURIOUS) {
                enterState(STATE_CURIOUS);
                lastDetectTime = now;
            }
            if (elapsed > 60000) {
                enterState(STATE_SLEEP);
            }
            break;

        case STATE_CURIOUS:
            // 头转向检测方向
            {
                int dir = sensorDirection();
                int yawTarget = HEAD_YAW_CENTER + dir * 25;
                headYaw.write(constrain(yawTarget, 45, 135));
                headPitch.write(HEAD_PITCH_CENTER - 10); // 微微抬头
            }

            if (distMin < DIST_GREET) {
                enterState(STATE_GREETING);
            } else if (distMin > DIST_FAR) {
                if (now - lastDetectTime > IDLE_TIMEOUT) {
                    enterState(STATE_SAD);
                }
            } else {
                lastDetectTime = now;
            }
            break;

        case STATE_GREETING:
            // 举一只手挥动
            {
                int wave = ARM_LEFT_UP - 20 * sin(elapsed / 200.0);
                armLeft.write(constrain(wave, ARM_LEFT_DOWN, ARM_LEFT_UP));
                headPitch.write(HEAD_PITCH_CENTER - 15);
            }
            // 前进一小步
            if (elapsed < 500) {
                trackForward(MOTOR_SPEED_LOW);
            } else {
                trackStop();
            }

            if (distMin < DIST_CLOSE) {
                enterState(STATE_EXCITED);
            } else if (distMin > DIST_CURIOUS) {
                if (now - lastDetectTime > IDLE_TIMEOUT) {
                    enterState(STATE_SAD);
                }
            } else {
                lastDetectTime = now;
            }
            break;

        case STATE_EXCITED:
            // 双手举起快速点头
            armLeft.write(ARM_LEFT_UP);
            armRight.write(ARM_RIGHT_UP);
            {
                int nod = HEAD_PITCH_CENTER - 20 + 15 * sin(elapsed / 150.0);
                headPitch.write(constrain(nod, 50, 130));
                int shake = HEAD_YAW_CENTER + 10 * sin(elapsed / 200.0);
                headYaw.write(shake);
            }

            if (distMin > DIST_GREET) {
                lastDetectTime = now;
                enterState(STATE_GREETING);
            }
            if (elapsed > 5000) {
                enterState(STATE_GREETING);
            }
            break;

        case STATE_SAD:
            // 头低下，手放下
            headPitch.write(HEAD_PITCH_CENTER + 20);
            headYaw.write(HEAD_YAW_CENTER);
            armLeft.write(ARM_LEFT_DOWN);
            armRight.write(ARM_RIGHT_DOWN);
            // 后退一小步
            if (elapsed < 300) {
                trackBackward(MOTOR_SPEED_LOW);
            } else {
                trackStop();
            }

            if (elapsed > 3000) {
                enterState(STATE_IDLE);
            }
            if (distMin < DIST_CURIOUS) {
                enterState(STATE_CURIOUS);
                lastDetectTime = now;
            }
            break;

        case STATE_SLEEP:
            headPitch.write(HEAD_PITCH_CENTER + 25);
            armLeft.write(ARM_LEFT_DOWN);
            armRight.write(ARM_RIGHT_DOWN);
            trackStop();

            if (distMin < DIST_CURIOUS) {
                enterState(STATE_CURIOUS);
                lastDetectTime = now;
            }
            break;
    }
}

#endif
