#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "config.h"
#include "behavior.h"

SPIClass* hspi = NULL;
Adafruit_ST7735* tft = NULL;
GFXcanvas16* canvas = NULL;

// 颜色定义
#define COL_BG      0x0000  // 黑色背景
#define COL_EYE     0xFFE0  // 瓦力暖黄色眼睛
#define COL_HAPPY   0x07E0  // 绿色
#define COL_LOVE    0xF800  // 红色
#define COL_SAD     0x001F  // 蓝色
#define COL_WHITE   0xFFFF
#define COL_GRAY    0x7BEF

// 表情绘制参数 (1.8" 128×160)
#define EYE_Y        55
#define EYE_L_X      40
#define EYE_R_X      88
#define EYE_RADIUS   16
#define PUPIL_RADIUS  6

static unsigned long lastBlink = 0;
static bool blinking = false;
static unsigned long blinkStart = 0;

void displayInit() {
    hspi = new SPIClass(HSPI);
    hspi->begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);

    tft = new Adafruit_ST7735(hspi, TFT_CS, TFT_DC, TFT_RST);
    tft->initR(INITR_BLACKTAB);
    tft->setRotation(0);
    tft->fillScreen(COL_BG);

    canvas = new GFXcanvas16(SCREEN_W, SCREEN_H);

    ledcAttach(TFT_BLK, 5000, 8);
    ledcWrite(TFT_BLK, 200);
}

void drawEye(int cx, int cy, int r, float offsetX, float offsetY, bool closed) {
    if (closed) {
        canvas->drawFastHLine(cx - r, cy, r * 2, COL_EYE);
        canvas->drawFastHLine(cx - r + 3, cy - 1, r * 2 - 6, COL_EYE);
        canvas->drawFastHLine(cx - r + 3, cy + 1, r * 2 - 6, COL_EYE);
        return;
    }
    // 眼眶
    canvas->drawCircle(cx, cy, r, COL_EYE);
    canvas->drawCircle(cx, cy, r - 1, COL_EYE);
    // 瞳孔（跟随偏移）
    int px = cx + (int)(offsetX * (r - PUPIL_RADIUS - 4));
    int py = cy + (int)(offsetY * (r - PUPIL_RADIUS - 4));
    canvas->fillCircle(px, py, PUPIL_RADIUS, COL_EYE);
}

void drawExpression(int expr, unsigned long t) {
    canvas->fillScreen(COL_BG);

    // 眨眼逻辑
    unsigned long now = millis();
    if (!blinking && now - lastBlink > BLINK_INTERVAL + random(2000)) {
        blinking = true;
        blinkStart = now;
        lastBlink = now;
    }
    bool eyesClosed = blinking && (now - blinkStart < 150);
    if (blinking && now - blinkStart > 150) blinking = false;

    // 瞳孔微动
    float ox = 0.1 * sin(t / 1500.0);
    float oy = 0.05 * cos(t / 2000.0);

    switch (expr) {
        case 0: // IDLE - 正常眼睛
            drawEye(EYE_L_X, EYE_Y, EYE_RADIUS, ox, oy, eyesClosed);
            drawEye(EYE_R_X, EYE_Y, EYE_RADIUS, ox, oy, eyesClosed);
            break;

        case 1: // CURIOUS - 眼睛睁大
            drawEye(EYE_L_X, EYE_Y, EYE_RADIUS + 5, ox * 2, -0.2, eyesClosed);
            drawEye(EYE_R_X, EYE_Y, EYE_RADIUS + 5, ox * 2, -0.2, eyesClosed);
            break;

        case 2: // GREETING - 笑眼（弯弯的）
            canvas->drawCircle(EYE_L_X, EYE_Y + 10, EYE_RADIUS, COL_EYE);
            canvas->fillRect(EYE_L_X - EYE_RADIUS, EYE_Y + 10 - EYE_RADIUS, EYE_RADIUS * 2, EYE_RADIUS, COL_BG);
            canvas->drawCircle(EYE_R_X, EYE_Y + 10, EYE_RADIUS, COL_EYE);
            canvas->fillRect(EYE_R_X - EYE_RADIUS, EYE_Y + 10 - EYE_RADIUS, EYE_RADIUS * 2, EYE_RADIUS, COL_BG);
            break;

        case 3: // EXCITED - 爱心眼
            for (int i = 0; i < 2; i++) {
                int hx = (i == 0) ? EYE_L_X : EYE_R_X;
                canvas->fillCircle(hx - 5, EYE_Y - 3, 6, COL_LOVE);
                canvas->fillCircle(hx + 5, EYE_Y - 3, 6, COL_LOVE);
                canvas->fillTriangle(hx - 11, EYE_Y, hx + 11, EYE_Y, hx, EYE_Y + 12, COL_LOVE);
            }
            break;

        case 4: // SAD - 耷拉眼
            drawEye(EYE_L_X, EYE_Y + 5, EYE_RADIUS - 3, 0, 0.3, eyesClosed);
            drawEye(EYE_R_X, EYE_Y + 5, EYE_RADIUS - 3, 0, 0.3, eyesClosed);
            // 下弯眉毛
            canvas->drawLine(EYE_L_X - 20, EYE_Y - 30, EYE_L_X + 20, EYE_Y - 25, COL_EYE);
            canvas->drawLine(EYE_R_X - 20, EYE_Y - 25, EYE_R_X + 20, EYE_Y - 30, COL_EYE);
            break;

        case 5: // SLEEP - ZZZ
            canvas->drawFastHLine(EYE_L_X - 10, EYE_Y, 20, COL_EYE);
            canvas->drawFastHLine(EYE_R_X - 10, EYE_Y, 20, COL_EYE);
            canvas->setTextSize(1);
            canvas->setTextColor(COL_GRAY);
            int zx = 95 + 3 * sin(t / 800.0);
            canvas->setCursor(zx, 30);
            canvas->print("Z");
            canvas->setCursor(zx + 8, 22);
            canvas->print("z");
            canvas->setCursor(zx + 14, 16);
            canvas->print("z");
            break;
    }

    // 底部信息栏（时间 + 状态）
    canvas->setTextSize(1);
    canvas->setTextColor(COL_GRAY);
    canvas->setCursor(10, SCREEN_H - 20);
    // 时间会由 network 模块更新
    extern char timeStr[16];
    extern char weatherStr[32];
    canvas->print(timeStr);
    canvas->setCursor(10, SCREEN_H - 10);
    canvas->print(weatherStr);
}

void displayRender() {
    unsigned long t = millis();
    int expr = getExpression();
    drawExpression(expr, t);
    tft->drawRGBBitmap(0, 0, canvas->getBuffer(), SCREEN_W, SCREEN_H);
}

#endif
