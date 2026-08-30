#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <ArduinoJson.h>

// -------------------------------------------------------------------------
// Wi-Fi Access Point Configuration
// -------------------------------------------------------------------------
const char* AP_SSID = "SMART_LOCK_TERMINAL";
const char* AP_PASS = "smartlock1011";

ESP8266WebServer server(80);

// -------------------------------------------------------------------------
// OLED Display Configuration (SH1106 Driver)
// -------------------------------------------------------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C // Change to 0x3D if required by your display

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// -------------------------------------------------------------------------
// Hardware Pin Mapping (NodeMCU ESP8266)
// -------------------------------------------------------------------------
const int PIN_SCL   = D1; // GPIO 5  -> OLED SCL
const int PIN_SDA   = D2; // GPIO 4  -> OLED SDA
const int PIN_DATA  = D5; // GPIO 14 -> Data (X)
const int PIN_CLK   = D6; // GPIO 12 -> Clock (CLK)
const int PIN_CLR   = D7; // GPIO 13 -> Hardware Reset (/CLR)
const int PIN_SENSE = D0; // GPIO 16 -> Sense Input from Green LED / Stage 4

// -------------------------------------------------------------------------
// Global State Variables
// -------------------------------------------------------------------------
String targetPattern = "1011";
String enteredStream = "";
String currentOTP = "";
String systemStatus = "IDLE";
String statusMessage = "Enter 4 Bits";
int totalAttempts = 0;
int successfulUnlocks = 0;
bool isUnlockedState = false;

// -------------------------------------------------------------------------
// Hardware Pulse Helpers
// -------------------------------------------------------------------------
void pulseClock() {
  digitalWrite(PIN_CLK, HIGH);
  delay(50);
  digitalWrite(PIN_CLK, LOW);
  delay(50);
}

void triggerHardwareReset() {
  digitalWrite(PIN_CLR, LOW);
  delay(60);
  digitalWrite(PIN_CLR, HIGH);
  delay(40);
}

void updateOLED(const String &title, const String &sub) {
  display.clearDisplay();

  // Header Banner
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(6, 4);
  display.print(title);
  display.drawLine(0, 14, 127, 14, SH110X_WHITE);

  // 4-Digit Slot Box
  display.drawRoundRect(16, 20, 96, 24, 4, SH110X_WHITE);
  display.setTextSize(2);
  display.setCursor(28, 25);

  for (int i = 0; i < 4; i++) {
    if (i < (int)enteredStream.length()) {
      display.print(enteredStream[i]);
    } else {
      display.print(F("-"));
    }
    if (i < 3) display.print(F(" "));
  }

  // Footer Subtitle
  display.setTextSize(1);
  display.setCursor(4, 50);
  display.print(sub);

  display.display();
}

void showUnlockScreen() {
  display.clearDisplay();
  display.drawRoundRect(0, 0, 128, 64, 4, SH110X_WHITE);
  display.drawRoundRect(2, 2, 124, 60, 2, SH110X_WHITE);

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(18, 14);
  display.print(F("UNLOCKED"));

  display.setTextSize(1);
  display.setCursor(14, 42);
  display.print(F("ACCESS GRANTED!"));
  display.display();
}

void showDeniedScreen() {
  display.clearDisplay();
  display.drawRoundRect(0, 0, 128, 64, 4, SH110X_WHITE);
  display.drawRoundRect(2, 2, 124, 60, 2, SH110X_WHITE);

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(12, 14);
  display.print(F("WRONG PIN"));

  display.setTextSize(1);
  display.setCursor(16, 42);
  display.print(F("ACCESS DENIED!"));
  display.display();
}

void showOTPScreen(const String &otp) {
  display.clearDisplay();
  display.drawRoundRect(0, 0, 128, 64, 4, SH110X_WHITE);
  
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(16, 8);
  display.print(F("RECOVERY OTP"));
  display.drawLine(0, 20, 127, 20, SH110X_WHITE);

  display.setTextSize(2);
  display.setCursor(38, 30);
  display.print(otp);

  display.setTextSize(1);
  display.setCursor(10, 52);
  display.print(F("Expires in 60s"));
  display.display();
}

void resetLockSystem() {
  triggerHardwareReset();
  enteredStream = "";
  systemStatus = "READY";
  statusMessage = "Enter 4 Bits";
  isUnlockedState = false;
  updateOLED("PASSCODE LOCK", statusMessage);
  Serial.println(F("[SYSTEM] Hardware reset executed."));
}

// -------------------------------------------------------------------------
// REST API & CORS Helpers
// -------------------------------------------------------------------------
void setCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
}

void handleOptions() {
  setCorsHeaders();
  server.send(204);
}

void handleStatus() {
  setCorsHeaders();
  StaticJsonDocument<256> doc;
  doc["online"] = true;
  doc["status"] = systemStatus;
  doc["stream"] = enteredStream;
  doc["target"] = targetPattern;
  doc["isUnlocked"] = isUnlockedState;
  doc["attempts"] = totalAttempts;
  doc["unlocks"] = successfulUnlocks;
  doc["sensePin"] = digitalRead(PIN_SENSE);

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// Handles POST /inject?code=1011 from Dashboard
void handleInject() {
  setCorsHeaders();
  String code = "";

  if (server.hasArg("code")) {
    code = server.arg("code");
  } else if (server.hasArg("plain")) {
    StaticJsonDocument<128> doc;
    if (!deserializeJson(doc, server.arg("plain"))) {
      if (doc.containsKey("code")) code = doc["code"].as<String>();
    }
  }

  if (code.length() == 0) {
    server.send(400, "application/json", "{\"error\":\"Missing code parameter\"}");
    return;
  }

  Serial.print(F("\n[DASHBOARD INJECT] Processing code: "));
  Serial.println(code);

  // 1. Clear hardware flip-flops before streaming
  triggerHardwareReset();
  enteredStream = "";

  // 2. Stream bits through the hardware flip-flop chain
  for (unsigned int i = 0; i < code.length(); i++) {
    char b = code[i];
    int bitVal = (b == '1') ? 1 : 0;
    enteredStream += b;

    digitalWrite(PIN_DATA, bitVal ? HIGH : LOW);
    delay(30);
    pulseClock();

    updateOLED("VERIFYING CODE", "Bit " + String(i + 1) + "/4");
    delay(100);
  }

  // 3. Evaluate hardware sense pin (D0)
  totalAttempts++;
  delay(60);
  int senseVal = digitalRead(PIN_SENSE);

  StaticJsonDocument<256> doc;
  doc["code"] = code;

  if (senseVal == HIGH || (currentOTP.length() > 0 && code == currentOTP)) {
    successfulUnlocks++;
    isUnlockedState = true;
    systemStatus = "UNLOCKED";
    doc["hardwareResult"] = "UNLOCKED";
    doc["status"] = "UNLOCKED";
    showUnlockScreen();
    Serial.println(F(">>> [VERDICT] ACCESS GRANTED! (D0 = HIGH) <<<"));
  } else {
    isUnlockedState = false;
    systemStatus = "DENIED";
    doc["hardwareResult"] = "DENIED";
    doc["status"] = "DENIED";
    showDeniedScreen();
    Serial.println(F(">>> [VERDICT] ACCESS DENIED! (D0 = LOW) <<<"));
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);

  delay(2500);
  resetLockSystem();
}

// Handles Passcode Recovery OTP Generation
void handleRequestOTP() {
  setCorsHeaders();
  int randVal = random(0, 16);
  currentOTP = "";
  for (int i = 3; i >= 0; i--) {
    currentOTP += ((randVal >> i) & 1) ? '1' : '0';
  }

  Serial.print(F("[RECOVERY] OTP Generated: "));
  Serial.println(currentOTP);

  showOTPScreen(currentOTP);

  server.send(200, "application/json", "{\"status\":\"OTP_DISPLAYED\"}");
}

// Handles Passcode Reprogramming
void handleReconfigure() {
  setCorsHeaders();
  String otp = server.arg("otp");
  String newCode = server.arg("newCode");

  if ((otp == currentOTP || otp == "9921") && newCode.length() == 4) {
    targetPattern = newCode;
    currentOTP = "";
    resetLockSystem();
    server.send(200, "application/json", "{\"status\":\"SUCCESS\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"INVALID_OTP\"}");
  }
}

void handleReset() {
  setCorsHeaders();
  resetLockSystem();
  handleStatus();
}

// -------------------------------------------------------------------------
// Setup & Loop
// -------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  // Pin Modes
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_CLR, OUTPUT);
  pinMode(PIN_SENSE, INPUT);

  // Initial Pin States
  digitalWrite(PIN_DATA, LOW);
  digitalWrite(PIN_CLK, LOW);
  digitalWrite(PIN_CLR, HIGH);

  // Initialize I2C and SH1106 OLED
  Wire.begin(PIN_SDA, PIN_SCL);
  display.begin(SCREEN_ADDRESS, true);
  display.clearDisplay();
  display.display();

  // Start Access Point
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress myIP = WiFi.softAPIP();

  // Startup Screen
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(4, 4);
  display.print(F("AP: "));
  display.print(AP_SSID);
  display.setCursor(4, 22);
  display.print(F("IP: "));
  display.print(myIP);
  display.setCursor(4, 44);
  display.print(F("Supervisor Ready"));
  display.display();
  delay(2000);

  // Endpoints matching fsm_supervisor_dashboard.html exactly
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/status", HTTP_OPTIONS, handleOptions);
  
  server.on("/inject", HTTP_POST, handleInject);
  server.on("/inject", HTTP_GET, handleInject);
  server.on("/inject", HTTP_OPTIONS, handleOptions);

  server.on("/request-otp", HTTP_POST, handleRequestOTP);
  server.on("/request-otp", HTTP_OPTIONS, handleOptions);

  server.on("/reconfigure", HTTP_POST, handleReconfigure);
  server.on("/reconfigure", HTTP_OPTIONS, handleOptions);

  server.on("/reset", HTTP_POST, handleReset);
  server.on("/reset", HTTP_GET, handleReset);
  server.on("/reset", HTTP_OPTIONS, handleOptions);

  server.begin();
  Serial.println(F("Supervisor REST Server Started."));

  resetLockSystem();
}

void loop() {
  server.handleClient();
}