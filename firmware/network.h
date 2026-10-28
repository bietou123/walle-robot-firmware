#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <time.h>
#include <LittleFS.h>
#include "config.h"

WebServer server(80);

char timeStr[16] = "00:00";
char weatherStr[32] = "";
char wifiSSID[64] = "";
char wifiPass[64] = "";

bool wifiConnected = false;
unsigned long lastWeatherUpdate = 0;

void loadWifiConfig() {
    if (!LittleFS.begin(true)) return;
    File f = LittleFS.open("/wifi.cfg", "r");
    if (!f) return;
    String ssid = f.readStringUntil('\n');
    String pass = f.readStringUntil('\n');
    f.close();
    ssid.trim();
    pass.trim();
    ssid.toCharArray(wifiSSID, sizeof(wifiSSID));
    pass.toCharArray(wifiPass, sizeof(wifiPass));
}

void saveWifiConfig(const char* ssid, const char* pass) {
    File f = LittleFS.open("/wifi.cfg", "w");
    if (!f) return;
    f.println(ssid);
    f.println(pass);
    f.close();
}

void networkConnect() {
    if (strlen(wifiSSID) == 0) return;
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiSSID, wifiPass);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT) {
        delay(200);
    }
    wifiConnected = (WiFi.status() == WL_CONNECTED);
    if (wifiConnected) {
        configTime(GMT_OFFSET, 0, NTP_SERVER);
    }
}

void updateTime() {
    struct tm ti;
    if (getLocalTime(&ti, 100)) {
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d", ti.tm_hour, ti.tm_min);
    }
}

void updateWeather() {
    if (!wifiConnected) return;
    if (millis() - lastWeatherUpdate < WEATHER_INTERVAL && lastWeatherUpdate > 0) return;

    // 简化版天气获取
    String url = "http://api.openweathermap.org/data/2.5/weather?q=";
    url += WEATHER_CITY;
    url += "&appid=";
    url += WEATHER_API_KEY;
    url += "&units=metric&lang=zh_cn";

    HTTPClient http;
    http.begin(url);
    int code = http.GET();
    if (code == 200) {
        String payload = http.getString();
        // 简单解析温度 (找 "temp": 后面的数字)
        int idx = payload.indexOf("\"temp\":");
        if (idx > 0) {
            String tempS = payload.substring(idx + 7, payload.indexOf(",", idx + 7));
            snprintf(weatherStr, sizeof(weatherStr), "%s C", tempS.c_str());
        }
    }
    http.end();
    lastWeatherUpdate = millis();
}

// ==================== Web Server ====================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>WALL-E Control</title>
<style>
body{font-family:sans-serif;background:#1a1a1a;color:#fff;padding:20px;max-width:600px;margin:0 auto}
h1{color:#f5a623;text-align:center}
.card{background:#2a2a2a;border-radius:12px;padding:16px;margin:12px 0}
.card h3{color:#f5a623;margin:0 0 12px 0}
input,select{width:100%;padding:8px;margin:4px 0;border-radius:6px;border:1px solid #444;background:#333;color:#fff;box-sizing:border-box}
button{background:#f5a623;color:#000;border:none;padding:10px 20px;border-radius:6px;cursor:pointer;font-weight:bold;margin:4px}
button:active{background:#d4891a}
.motor-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.slider{width:100%}
</style>
</head>
<body>
<h1>WALL-E</h1>

<div class="card">
<h3>WiFi</h3>
<input id="ssid" placeholder="SSID">
<input id="pass" type="password" placeholder="Password">
<button onclick="saveWifi()">Connect</button>
<p id="wifiStatus"></p>
</div>

<div class="card">
<h3>Motors</h3>
<div class="motor-grid">
<div>Head Yaw<br><input type="range" min="45" max="135" value="90" oninput="setServo('yaw',this.value)"></div>
<div>Head Pitch<br><input type="range" min="45" max="135" value="90" oninput="setServo('pitch',this.value)"></div>
<div>Left Arm<br><input type="range" min="30" max="150" value="30" oninput="setServo('armL',this.value)"></div>
<div>Right Arm<br><input type="range" min="30" max="150" value="150" oninput="setServo('armR',this.value)"></div>
</div>
<br>
<button onclick="track('fwd')">Forward</button>
<button onclick="track('back')">Backward</button>
<button onclick="track('left')">Turn L</button>
<button onclick="track('right')">Turn R</button>
<button onclick="track('stop')">STOP</button>
</div>

<div class="card">
<h3>Expression</h3>
<button onclick="setExpr(0)">Idle</button>
<button onclick="setExpr(1)">Curious</button>
<button onclick="setExpr(2)">Happy</button>
<button onclick="setExpr(3)">Love</button>
<button onclick="setExpr(4)">Sad</button>
<button onclick="setExpr(5)">Sleep</button>
</div>

<div class="card">
<h3>Info</h3>
<p>IP: <span id="ip">-</span></p>
<p>Distance L: <span id="distL">-</span> cm</p>
<p>Distance R: <span id="distR">-</span> cm</p>
<p>State: <span id="state">-</span></p>
</div>

<script>
function saveWifi(){fetch('/wifi?ssid='+encodeURIComponent(document.getElementById('ssid').value)+'&pass='+encodeURIComponent(document.getElementById('pass').value)).then(r=>r.text()).then(t=>document.getElementById('wifiStatus').textContent=t)}
function setServo(name,val){fetch('/servo?'+name+'='+val)}
function track(dir){fetch('/track?dir='+dir)}
function setExpr(id){fetch('/expr?id='+id)}
function poll(){fetch('/status').then(r=>r.json()).then(d=>{document.getElementById('distL').textContent=d.dL;document.getElementById('distR').textContent=d.dR;document.getElementById('state').textContent=d.st;document.getElementById('ip').textContent=d.ip}).catch(()=>{})}
setInterval(poll,1000);
poll();
</script>
</body>
</html>
)rawliteral";

extern float distLeft, distRight;
extern State currentState;

void setupWebServer() {
    server.on("/", []() {
        server.send(200, "text/html", INDEX_HTML);
    });

    server.on("/wifi", []() {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        if (ssid.length() > 0) {
            ssid.toCharArray(wifiSSID, sizeof(wifiSSID));
            pass.toCharArray(wifiPass, sizeof(wifiPass));
            saveWifiConfig(wifiSSID, wifiPass);
            server.send(200, "text/plain", "Saved. Reconnecting...");
            delay(500);
            networkConnect();
        } else {
            server.send(400, "text/plain", "Missing SSID");
        }
    });

    server.on("/servo", []() {
        if (server.hasArg("yaw")) headYaw.write(server.arg("yaw").toInt());
        if (server.hasArg("pitch")) headPitch.write(server.arg("pitch").toInt());
        if (server.hasArg("armL")) armLeft.write(server.arg("armL").toInt());
        if (server.hasArg("armR")) armRight.write(server.arg("armR").toInt());
        server.send(200, "text/plain", "OK");
    });

    server.on("/track", []() {
        String dir = server.arg("dir");
        if (dir == "fwd") trackForward(MOTOR_SPEED_MED);
        else if (dir == "back") trackBackward(MOTOR_SPEED_MED);
        else if (dir == "left") trackTurnLeft(MOTOR_SPEED_MED);
        else if (dir == "right") trackTurnRight(MOTOR_SPEED_MED);
        else trackStop();
        server.send(200, "text/plain", "OK");
    });

    server.on("/expr", []() {
        int id = server.arg("id").toInt();
        enterState((State)id);
        server.send(200, "text/plain", "OK");
    });

    server.on("/status", []() {
        String json = "{\"dL\":";
        json += String(distLeft, 1);
        json += ",\"dR\":";
        json += String(distRight, 1);
        json += ",\"st\":\"";
        const char* stNames[] = {"IDLE","CURIOUS","GREETING","EXCITED","SAD","SLEEP"};
        json += stNames[currentState];
        json += "\",\"ip\":\"";
        json += WiFi.localIP().toString();
        json += "\"}";
        server.send(200, "application/json", json);
    });

    server.begin();
}

#endif
