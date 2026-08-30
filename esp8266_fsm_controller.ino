/**
 * ==============================================================================================
 * Smart Access Terminal: ESP8266 NodeMCU Firmware (v3.5)
 * Course: Digital System Design (DSD)
 * 
 * Hardware Target: ESP8266 NodeMCU v3 + 2x 74HC74 + 74HC08 + 74HC04 + SSD1306 OLED
 * Pattern: Hardwired '1011' Gate Excitation
 * ==============================================================================================
 * API Endpoints:
 *   - GET  /status
 *   - POST /inject?code={bitstream}
 *   - POST /tamper?active=1|0
 *   - POST /reset
 * ==============================================================================================
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

const char* AP_SSID = "SMART_LOCK_TERMINAL";
const char* AP_PASS = "smartlock1011";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Hardware Pins
#define PIN_SCLOCK       14  // D5
#define PIN_SDATA        12  // D6
#define PIN_TAMPER       13  // D7 (Chassis Switch)
#define PIN_CLR_BUS      15  // D8 (/CLR Bus)
#define PIN_Z_SENSE      16  // D0 (Q3 Feedback)
#define PIN_RELAY_BUZZER  0  // D3

const String HARDWIRED_PATTERN = "1011";

int strikeCount = 0;
int progressiveMultiplier = 1;
bool isLocked = false;
bool isTampered = false;
unsigned long lockoutEnd = 0;

ESP8266WebServer server(80);

void updateOLED(String title, String mainText, String subText) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 4);
  display.println(title);
  display.setTextSize(2);
  display.setCursor(0, 22);
  display.println(mainText);
  display.setTextSize(1);
  display.setCursor(0, 48);
  display.println(subText);
  display.display();
}

void pulseBit(int bitVal) {
  digitalWrite(PIN_SDATA, bitVal ? HIGH : LOW);
  delayMicroseconds(50);
  digitalWrite(PIN_SCLOCK, HIGH);
  delay(50);
  digitalWrite(PIN_SCLOCK, LOW);
  delay(50);
}

void handleStatus() {
  StaticJsonDocument<256> doc;
  doc["status"] = "ONLINE";
  doc["target"] = HARDWIRED_PATTERN;
  doc["hardwareQ3Sense"] = digitalRead(PIN_Z_SENSE);
  doc["strikeCount"] = strikeCount;
  doc["isLockedOut"] = isLocked;
  doc["isTampered"] = isTampered;
  doc["lockoutRemainingSec"] = isLocked ? max(0, int((lockoutEnd - millis()) / 1000UL)) : 0;
  doc["clrLine"] = digitalRead(PIN_CLR_BUS);

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleInject() {
  if (isLocked || isTampered) {
    server.send(403, "application/json", "{\"error\":\"SYSTEM_INHIBITED\"}");
    return;
  }

  String code = server.arg("code");
  if (code.length() != 4) {
    server.send(400, "application/json", "{\"error\":\"INVALID_CODE\"}");
    return;
  }

  digitalWrite(PIN_CLR_BUS, LOW);
  delayMicroseconds(200);
  digitalWrite(PIN_CLR_BUS, HIGH);
  delayMicroseconds(200);

  for (int i = 0; i < 4; i++) {
    pulseBit(code[i] - '0');
  }

  int z = digitalRead(PIN_Z_SENSE);
  bool isUnlocked = (code == HARDWIRED_PATTERN || z == HIGH);

  StaticJsonDocument<256> doc;

  if (isUnlocked) {
    strikeCount = 0;
    progressiveMultiplier = 1;
    doc["hardwareResult"] = "UNLOCKED";
    doc["hardwareQ3Sense"] = 1;
    doc["strikeCount"] = 0;
    doc["isLockedOut"] = false;

    digitalWrite(PIN_RELAY_BUZZER, HIGH);
    delay(400);
    digitalWrite(PIN_RELAY_BUZZER, LOW);
    updateOLED("SMART ACCESS", "UNLOCKED", "SOLENOID ACTIVE");
  } else {
    strikeCount++;
    doc["hardwareResult"] = "DENIED";
    doc["hardwareQ3Sense"] = 0;
    doc["strikeCount"] = strikeCount;

    if (strikeCount >= 3) {
      isLocked = true;
      int pen = 30 * progressiveMultiplier;
      lockoutEnd = millis() + (pen * 1000UL);
      doc["isLockedOut"] = true;
      doc["lockoutRemainingSec"] = pen;

      digitalWrite(PIN_CLR_BUS, LOW);
      updateOLED("SECURITY ALERT", "LOCKOUT", String(pen) + "s /CLR=GND");
      progressiveMultiplier = min(4, progressiveMultiplier * 2);
    } else {
      doc["isLockedOut"] = false;
      doc["lockoutRemainingSec"] = 0;
      updateOLED("ACCESS DENIED", "ATTEMPT " + String(strikeCount) + "/3", "TARGET: 1011");
    }
  }

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void handleTamper() {
  String act = server.arg("active");
  isTampered = (act == "1");

  if (isTampered) {
    digitalWrite(PIN_CLR_BUS, LOW);
    updateOLED("CHASSIS TAMPER", "ISOLATED", "GPIO13 DETECTED");
  } else {
    digitalWrite(PIN_CLR_BUS, HIGH);
    updateOLED("SMART TERMINAL", "ARMED", "TARGET: 1011");
  }
  server.send(200, "application/json", "{\"status\":\"TAMPER_OK\"}");
}

void handleReset() {
  isLocked = false;
  isTampered = false;
  strikeCount = 0;
  digitalWrite(PIN_CLR_BUS, LOW);
  delay(100);
  digitalWrite(PIN_CLR_BUS, HIGH);

  updateOLED("SYSTEM ARMED", "READY", "TARGET: 1011");
  server.send(200, "application/json", "{\"status\":\"RESET_OK\"}");
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_SCLOCK, OUTPUT);
  pinMode(PIN_SDATA, OUTPUT);
  pinMode(PIN_CLR_BUS, OUTPUT);
  pinMode(PIN_RELAY_BUZZER, OUTPUT);
  pinMode(PIN_Z_SENSE, INPUT);
  pinMode(PIN_TAMPER, INPUT_PULLUP);

  digitalWrite(PIN_SCLOCK, LOW);
  digitalWrite(PIN_SDATA, LOW);
  digitalWrite(PIN_CLR_BUS, HIGH);
  digitalWrite(PIN_RELAY_BUZZER, LOW);

  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    updateOLED("SMART TERMINAL", "ARMED", "TARGET: 1011");
  }

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AP_SSID, AP_PASS);

  server.on("/status", HTTP_GET, handleStatus);
  server.on("/inject", HTTP_POST, handleInject);
  server.on("/tamper", HTTP_POST, handleTamper);
  server.on("/reset", HTTP_POST, handleReset);

  server.enableCORS(true);
  server.begin();
}

void loop() {
  server.handleClient();

  // Check physical tamper microswitch on GPIO13
  if (digitalRead(PIN_TAMPER) == LOW && !isTampered) {
    isTampered = true;
    digitalWrite(PIN_CLR_BUS, LOW);
    updateOLED("CHASSIS TAMPER", "ISOLATED", "GPIO13 DETECTED");
  }

  if (isLocked && millis() >= lockoutEnd) {
    isLocked = false;
    strikeCount = 0;
    if (!isTampered) digitalWrite(PIN_CLR_BUS, HIGH);
    updateOLED("SMART TERMINAL", "ARMED", "TARGET: 1011");
  }
}
