# 🚀 Smart FSM Access Terminal
### ⚡ DSD Micro Project -- Hybrid Synchronous Finite State Machine Sequence Detector

<div align="center">

![Architecture](https://img.shields.io/badge/Architecture-Synchronous%20FSM-2563EB?style=for-the-badge)
![ICs](https://img.shields.io/badge/ICs-74HC74%20%7C%2074HC08%20%7C%2074HC04-EA580C?style=for-the-badge)
![MCU](https://img.shields.io/badge/MCU-ESP8266%20NodeMCU-DC2626?style=for-the-badge)
![Firmware](https://img.shields.io/badge/Firmware-Arduino%20C%2B%2B-00979D?style=for-the-badge)
![Server](https://img.shields.io/badge/Mock%20Server-Python%203-3776AB?style=for-the-badge)
![Tests](https://img.shields.io/badge/Test%20Suite-Node.js-339933?style=for-the-badge)
![BOM](https://img.shields.io/badge/BOM%20Cost-Rs.788%20~USD%209.50-16A34A?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-6B7280?style=for-the-badge)
![SDG](https://img.shields.io/badge/SDG-9%20and%2012-7C3AED?style=for-the-badge)

</div>

> **📚 Course:** Digital System Design (DSD) Micro Project  
> **🔌 IC Stack:** 74HC74 + 74HC08 + 74HC04 + ESP8266 NodeMCU + SH1106 OLED  
> **💰 Total BOM:** Rs.788 | **🎯 Target Sequence:** 1-0-1-1 (reconfigurable) | **⚖️ License:** MIT  

---

## 📑 Table of Contents

1. [🔍 Project Overview](#1--project-overview)
2. [🏗️ System Architecture](#2-️-system-architecture)
3. [📐 FSM Mathematical Formulation](#3--fsm-mathematical-formulation)
4. [🛠️ Hardware Design](#4-️-hardware-design)
5. [📂 Repository Structure](#5--repository-structure)
6. [🚀 Getting Started](#6--getting-started)
7. [🌐 REST API Reference](#7--rest-api-reference)
8. [🧪 Automated Test Suite](#8--automated-test-suite)
9. [🛒 Bill of Materials](#9--bill-of-materials)
10. [🛡️ Safety Interlock Subsystems](#10-️-safety-interlock-subsystems)
11. [💡 Key Design Decisions](#11--key-design-decisions)
12. [🌍 SDG Alignment](#12--sdg-alignment)
13. [🎥 Demo and Media](#13--demo-and-media)
14. [⚙️ How It Works -- Step by Step](#14-️-how-it-works----step-by-step)
15. [👥 Team](#15--team)
16. [📜 License](#16--license)

---

## 1. 🔍 Project Overview

This project presents a pedagogical yet **industrial-grade Hybrid Synchronous Finite State Machine (FSM)** sequence detector built from discrete 7400-series CMOS ICs and supervised by an ESP8266 NodeMCU over a live Wi-Fi REST API. The design directly bridges foundational D flip-flop and gate theory with modern IoT SCADA supervisory architectures.

| Attribute | Detail |
|:---|:---|
| **📚 Course** | Digital System Design (DSD) Micro Project |
| **🎯 Detect Target** | `1-0-1-1` (dynamically reconfigurable via EEPROM at runtime) |
| **⚡ Core Logic ICs** | 74HC74 Dual D-FF, 74HC08 Quad AND, 74HC04 Hex Inverter |
| **🧠 Microcontroller** | ESP8266 NodeMCU v3 -- Tensilica 32-bit @ 80 MHz, 4 MB Flash |
| **📺 Display** | SSD1306 / SH1106 0.96-inch I2C OLED 128x64 px |
| **📡 Connectivity** | Wi-Fi Soft Access Point, REST API port 80 |
| **🔢 Encoding Scheme** | One-Hot (5 states: S0 to S4) |
| **⚙️ FSM Type** | Hybrid Moore/Mealy synchronous, positive-edge-triggered |
| **🛡️ Safety Layers** | Progressive lockout, tamper detection, OTP, duress protocol |
| **💵 BOM Cost** | Rs. 788.00 (approx. USD 9.50) |
| **🌍 SDG Alignment** | SDG 9 (Innovation) + SDG 12 (Responsible Consumption) |

---

## 2. 🏗️ System Architecture

The system is organized as **four hierarchical layers**:

```text
+---------------------------------------------------------+
| LAYER 4 -- Web Supervisor Dashboard (Browser UI)        |
| fsm_supervisor_dashboard.html                           |
| Code Injection | Status Monitor | Reconfigure | OTP     |
+----------------------------+----------------------------+
                             | HTTP REST (port 80 / 8080)
+----------------------------v----------------------------+
| LAYER 3 -- ESP8266 NodeMCU Supervisor Controller        |
| REST Endpoints: /inject /reset /status /reconfigure     |
| SH1106 OLED Display (I2C) + Hardware FSM Driver         |
+--------+------------------+---------+------------------+
         |                  |         |                  
    CLK (D5)          DATA (D6)     /CLR (D7)          
         |                  |         |                  
+--------v---+    +----------v-+    +--v---------------+
| 74HC74 #1  |    | 74HC04 U3  |    | 74HC74 #2        |
| D-FF U1A:Q0|    | Inverter   |    | D-FF U2A:Q2      |
| D-FF U1B:Q1|    | X -> ~X    |    | D-FF U2B:Q3 (Z)  |
+------+-----+    +------+-----+    +--------+---------+
       |                 |                   |
       +--------+--------+                   |
                |                            |
+---------------v----------------------------v---------+
| LAYER 2 -- 74HC08 Quad AND Gate Array (U4)            |
| Computes: D0 = X.~Q1.~Q2.~Q3                         |
|           D1 = Q0.~X                                 |
|           D2 = Q1.X                                  |
|           D3 = Q2.X                                  |
+------------------------------------------------------+
                             |
+----------------------------v-------------------------+
| LAYER 1 -- Physical Output                           |
| Red LEDs: Q0, Q1, Q2 state indicators                |
| Green LED: Q3 = Z = 1 (UNLOCKED)                     |
| Relay / Solenoid actuator on GPIO0 (D3)              |
+---------------------------------------------------------+
```

---

## 3. 📐 FSM Mathematical Formulation

### 3.1 State Definitions (One-Hot Encoded)

The FSM detects the non-overlapping binary sequence **1-0-1-1** using 5 states:

| State | Semantic Meaning | One-Hot (Q3 Q2 Q1 Q0) | On Input=0 | On Input=1 |
|:---:|:---|:---:|:---:|:---:|
| **S0** | 🛡️ System Armed / Idle Standby | `0 0 0 0` | Stay S0 | Advance to S1 |
| **S1** | 1️⃣ 1st bit matched (`1`) | `0 0 0 1` | Advance to S2 | Stay S1 |
| **S2** | 2️⃣ 2nd bit matched (`1-0`) | `0 0 1 0` | Return to S0 | Advance to S3 |
| **S3** | 3️⃣ 3rd bit matched (`1-0-1`) | `0 1 0 0` | Return to S0 | Advance to S4 |
| **S4** | 🔓 Sequence complete -- **UNLOCKED** (Z=1) | `1 0 0 0` | Return to S0 | Advance to S1 |

### 3.2 Complete State Transition Truth Table

| Present State | Input X | Next State | Q3+ | Q2+ | Q1+ | Q0+ | /CLR | Output Z |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| **S0** | 0 | S0 | 0 | 0 | 0 | 0 | 1 | 0 |
| **S0** | 1 | S1 | 0 | 0 | 0 | 1 | 1 | 0 |
| **S1** | 0 | S2 | 0 | 0 | 1 | 0 | 1 | 0 |
| **S1** | 1 | S1 | 0 | 0 | 0 | 1 | 1 | 0 |
| **S2** | 0 | S0 | 0 | 0 | 0 | 0 | 1 | 0 |
| **S2** | 1 | S3 | 0 | 1 | 0 | 0 | 1 | 0 |
| **S3** | 0 | S0 | 0 | 0 | 0 | 0 | 1 | 0 |
| **S3** | 1 | S4 | 1 | 0 | 0 | 0 | 1 | **1** |
| **S4** | 0 | S0 | 0 | 0 | 0 | 0 | 1 | **1** |
| **S4** | 1 | S1 | 0 | 0 | 0 | 1 | 1 | **1** |

### 3.3 Minimized Boolean Equations

Applying Karnaugh map minimization and one-hot excitation logic:

```text
  D0  =  X . ~Q1 . ~Q2 . ~Q3    -- Next-state for Q0 (S1: first 1 detected)
  D1  =  Q0 . ~X                 -- Next-state for Q1 (S2: 0 follows first 1)
  D2  =  Q1 . X                  -- Next-state for Q2 (S3: 1 after 1-0)
  D3  =  Q2 . X                  -- Next-state for Q3 (S4: UNLOCK - full 1011)

  Output  Z  =  Q3               -- Moore output: HIGH only when in state S4
```

**🔬 IC Realization:**
- `74HC08` (Quad 2-input AND) -- Computes the product terms for D0, D1, D2, D3
- `74HC04` (Hex Inverter) -- Provides complement signals ~X, ~Q1, ~Q2, ~Q3
- `74HC74` (Dual D-FF, positive-edge-triggered) -- Stores Q0, Q1, Q2, Q3 across clock edges

---

## 4. 🛠️ Hardware Design

### 4.1 Pin Mapping -- ESP8266 NodeMCU v3

| NodeMCU Pin | GPIO | Direction | Connected To | Signal |
|:---:|:---:|:---:|:---|:---|
| **D1** | GPIO5 | OUT | SSD1306 OLED SCL | I2C Clock |
| **D2** | GPIO4 | OUT | SSD1306 OLED SDA | I2C Data |
| **D3** | GPIO0 | OUT | Relay / Buzzer IN | Solenoid trigger |
| **D5** | GPIO14 | OUT | 74HC74 U1+U2 CLK pins | Master synchronous clock |
| **D6** | GPIO12 | OUT | 74HC04 Pin 1 (1A) | Serial data bit X |
| **D7** | GPIO13 | OUT | 74HC74 U1+U2 /CLR bus | Active-LOW master clear |
| **D0** | GPIO16 | IN | 74HC74 U2 Q3 (Pin 9) | Unlock sense (Z output) |

### 4.2 IC Interconnect Table

| IC | Pin | Signal | Connected To | Function |
|:---|:---:|:---|:---|:---|
| **74HC74 #1 (U1)** | 1, 13 | /CLR | ESP8266 D7 (GPIO13) | Active-LOW async clear |
| **74HC74 #1 (U1)** | 2 | D input (D0) | 74HC08 AND output | Next-state for Q0 |
| **74HC74 #1 (U1)** | 3, 11 | CLK | ESP8266 D5 (GPIO14) | Positive-edge master clock |
| **74HC74 #1 (U1)** | 4, 10 | /PRE | VCC (3.3V) | Disabled -- tied HIGH |
| **74HC74 #1 (U1)** | 5 | Q0 | 74HC08 input + Red LED 0 | State bit Q0 |
| **74HC74 #1 (U1)** | 9 | Q1 | 74HC08 input + Red LED 1 | State bit Q1 |
| **74HC74 #2 (U2)** | 1, 13 | /CLR | ESP8266 D7 (GPIO13) | Active-LOW async clear |
| **74HC74 #2 (U2)** | 5 | Q2 | 74HC08 input + Red LED 2 | State bit Q2 |
| **74HC74 #2 (U2)** | 9 | Q3 | ESP8266 D0 + Green LED | Unlock output Z |
| **74HC04 (U3)** | 1 (1A) | X | ESP8266 D6 (GPIO12) | Serial bit input |
| **74HC04 (U3)** | 2 (1Y) | ~X | 74HC08 D0 gate | Inverted serial bit |
| **74HC08 (U4)** | Various | D0-D3 | 74HC74 D inputs | Next-state logic |
| **SSD1306 OLED** | SDA, SCL | I2C | ESP8266 D2, D1 | Real-time display |

### 4.3 IC Pinout Reference

```text
  74HC74 Dual D-FF (14-DIP)        74HC08 Quad AND (14-DIP)
  +---+--+---+                     +---+--+---+
  |/CLR1  VCC|                     |1A     VCC|
  |1D     /CLR2|                   |1B      4B|
  |1CLK    2D|                     |1Y      4A|
  |/PRE1 2CLK|                     |2A      4Y|
  |1Q   /PRE2|                     |2B      3B|
  |/1Q    2Q |                     |2Y      3A|
  |GND   /2Q |                     |GND     3Y|
  +-----------+                    +-----------+
```

---

## 5. 📂 Repository Structure

```text
fsm-smart-access-terminal/
│
├── esp8266_fsm_controller/
│   └── esp8266_fsm_controller.ino     # 🔌 Arduino firmware: REST API, OLED, FSM hardware driver
│
├── fsm_supervisor_dashboard.html      # 💻 Browser supervisor panel: inject codes, monitor state, OTP
├── mock_esp8266_server.py             # 🐍 Python mock hardware server (stdlib only, zero pip deps)
├── test_fsm_engine.js                 # 🧪 Node.js: exhaustive 16-combination FSM matrix test suite
├── dsd_project_documentation.md       # 📖 Full academic report: math, BOM, SDG, state tables
├── hardware_wiring_guide.md           # 🔌 IC pinouts, interconnect table, breadboard assembly guide
├── launch_smartlock.bat               # ⚡ One-click Windows launcher (mock server + dashboard)
├── README.md                          # 📄 Project documentation
├── requirements.txt                   # 📋 Python dependencies (stdlib only)
├── CONTRIBUTING.md                    # 🤝 Contribution guidelines
├── LICENSE                            # ⚖️ MIT License
└── .gitignore                         # 🚫 Python, Node.js, Arduino build artifact exclusions
```

---

## 6. 🚀 Getting Started

### 🔹 Option A -- Full Hardware Deployment (Real ESP8266)

**Step 1: Clone the repository**
```bash
git clone https://github.com/navadiadharmik05-hub/Smart-FSM-Access-Terminal.git
cd Smart-FSM-Access-Terminal
```

**Step 2: Install Arduino IDE dependencies**

| Library | Install via Arduino IDE Library Manager |
|:---|:---|
| `ESP8266 Board Package` | Boards Manager > search ESP8266 > Install |
| `Adafruit SH110X` | Library Manager > search Adafruit SH110X |
| `Adafruit GFX Library` | Library Manager > search Adafruit GFX |
| `ArduinoJson` | Library Manager > search ArduinoJson by Benoit Blanchon |

**Step 3: Flash firmware**
1. Open: `esp8266_fsm_controller/esp8266_fsm_controller.ino`
2. Board: **NodeMCU 1.0 (ESP-12E Module)**
3. Upload Speed: `115200` | Flash Size: `4MB (FS:2MB OTA:~1019KB)`
4. Click **Upload** 🚀

**Step 4: Connect to device Wi-Fi**
- **SSID:** `SMART_LOCK_TERMINAL`
- **Password:** `smartlock1011`
- **Dashboard:** `http://192.168.4.1`

---

### 🔹 Option B -- Local Simulation (No Hardware Required)

**Step 1: Start the mock hardware server** *(Python stdlib -- zero pip dependencies)*
```bash
python mock_esp8266_server.py
```

*Output:*
```text
======================================================================
  SMART ACCESS TERMINAL -- MOCK HARDWARE SERVER (http://127.0.0.1:8080)
======================================================================
  Endpoints: GET /status | POST /inject | POST /tamper | POST /reset
```

**Step 2: Open the Supervisor Dashboard**
1. Open `fsm_supervisor_dashboard.html` in any modern web browser.
2. Set server URL to: `http://127.0.0.1:8080`
3. Use the dashboard to inject codes, monitor flip-flop states, and trigger OTP!

⚡ **Windows One-Click Launch:**
Double click `launch_smartlock.bat` to start the mock server and open the dashboard automatically.

---

### 🔹 Option C -- Run the Automated FSM Test Suite

```bash
node test_fsm_engine.js
```

*Expected output:*
```text
=================================================================
RUNNING 16-STATE VALIDATION MATRIX FOR TARGET: [1011]
=================================================================
 Index | Injected Code | Final State | Expected | Verdict
-------+---------------+-------------+----------+---------
     1 |          0000 |          S0 |   DENIED |  PASS
     2 |          0001 |          S1 |   DENIED |  PASS
   ...
    12 |          1011 |          S4 | S4 (Z=1) |  PASS
   ...
Test Result: 16/16 Combinations Passed (100% Coverage)
FSM Mathematics Verified: ZERO FALSE POSITIVES / FALSE NEGATIVES.

ALL RECONFIGURED FSM COMBINATIONS PASSED VERIFICATION WITH 100% INTEGRITY.
```

---

## 7. 🌐 REST API Reference

The ESP8266 firmware (and mock server) expose the following HTTP endpoints:

| Method | Endpoint | Parameters | Description |
|:---:|:---|:---|:---|
| `GET` | `/status` | *None* | Live hardware state, Q-values, strikes, uptime |
| `POST` | `/inject` | `?code=1011` | Serially clocks 4-bit code into the flip-flop chain |
| `POST` | `/reset` | *None* | Pulses `/CLR` LOW -- clears all flip-flops to S0 |
| `POST` | `/request-otp` | *None* | Generates 4-bit OTP shown only on OLED |
| `POST` | `/reconfigure` | `?otp=XXXX&newCode=YYYY` | Reprograms target pattern at runtime |
| `POST` | `/tamper` | `?active=1 or 0` | Simulates chassis tamper switch (mock server only) |

### 💻 Sample curl Commands

```bash
curl http://192.168.4.1/status
curl -X POST "http://192.168.4.1/inject?code=1011"
curl -X POST http://192.168.4.1/reset
curl -X POST http://192.168.4.1/request-otp
```

### 📋 Sample Status Response

```json
{
  "status": "ONLINE",
  "target": "1011",
  "currentState": "S4",
  "flipFlops": [1, 0, 0, 0],
  "hardwareQ3Sense": 1,
  "strikeCount": 0,
  "isLockedOut": false,
  "uptimeSeconds": 347
}
```

---

## 8. 🧪 Automated Test Suite

The Node.js test suite (`test_fsm_engine.js`) runs an exhaustive **16-combination input matrix** across 4 different reconfigurable target sequences -- total of 64 test cases:

| Target Sequence | Combos Tested | Pass Rate | False Positives | False Negatives |
|:---:|:---:|:---:|:---:|:---:|
| `1011` (primary) | 16 / 16 | 100% | 0 | 0 |
| `1100` | 16 / 16 | 100% | 0 | 0 |
| `0101` | 16 / 16 | 100% | 0 | 0 |
| `1111` | 16 / 16 | 100% | 0 | 0 |
| **TOTAL** | **64 / 64** | **100%** | **0** | **0** |

---

## 9. 🛒 Bill of Materials

| Component | Specification | Qty | Unit (Rs.) | Total (Rs.) |
|:---|:---|:---:|:---:|:---:|
| **ESP8266 NodeMCU v3** | Tensilica 32-bit @ 80 MHz, Wi-Fi 802.11 b/g/n, 4MB Flash | 1 | 280.00 | 280.00 |
| **74HC74 Dual D-FF** | High-Speed CMOS, 14-DIP, 3.3V/5V compatible | 2 | 22.00 | 44.00 |
| **74HC08 Quad AND Gate** | 2-Input AND Gate, 14-DIP | 1 | 18.00 | 18.00 |
| **74HC04 Hex Inverter** | 6-Channel NOT Gate, 14-DIP | 1 | 15.00 | 15.00 |
| **SSD1306 I2C OLED** | 0.96-inch Monochrome 128x64 px | 1 | 190.00 | 190.00 |
| **5V Relay Module** | Optocoupler Isolated, 10A 250VAC rating | 1 | 65.00 | 65.00 |
| **LED Indicators** | 3mm Diffused: 3x Red (state), 1x Green (unlock) | 4 | 2.00 | 8.00 |
| **Discrete Passives** | 330Ω + 10kΩ resistors & decoupling capacitors | 8 | 1.00 | 8.00 |
| **Breadboard + Jumpers** | 830-point solderless + assorted wire kit | 1 | 160.00 | 160.00 |
| **TOTAL** | | | | **Rs. 788.00 (~USD 9.50)** |

---

## 10. 🛡️ Safety Interlock Subsystems

The system implements four independent hardware safety layers:

### 🔒 Layer 1 -- Progressive Lockout Penalty

| Strike Count | Hardware Action | /CLR State | Penalty Duration |
|:---:|:---|:---:|:---:|
| **1st wrong code** | Warning logged to serial | HIGH (normal) | None |
| **2nd wrong code** | Warning logged to serial | HIGH (normal) | None |
| **3rd wrong code** | ⚠️ LOCKOUT ENGAGED | **LOW (GND)** | 30 seconds |
| **4th+ (post reset)** | 🚫 Escalating lockout | **LOW (GND)** | 60s -> 120s (doubles) |

> When locked out, the `/CLR` bus is held permanently at GND, instantly discharging all internal master-slave latches and ignoring external clock pulses until the timer expires.

### 🚨 Layer 2 -- Chassis Tamper Isolation (GPIO13)
- Physical microswitch on enclosure wired to GPIO13 (active LOW).
- Tamper triggers a non-maskable interrupt pulling `/CLR` LOW immediately.
- Acoustic buzzer activates; all REST responses flag `isTampered: true`.

### 🔕 Layer 3 -- Duress Silent Panic Protocol
- Reserved duress code unlocks the gate mechanism visually with zero audible alerts.
- Dispatches a silent encrypted incident packet to the supervisor console in real time.

### 🔑 Layer 4 -- Dynamic 60-Second Single-Use OTP
- Generates a cryptographically-seeded 4-bit OTP displayed **ONLY** on the physical OLED.
- Enables temporary field-technician access without exposing the master passcode.
- Invalids automatically after single-use or after 60 seconds.

---

## 11. 💡 Key Design Decisions

| Design Choice | Engineering Rationale |
|:---|:---|
| **One-Hot Encoding** | Eliminates decoder complexity. Each state bit directly drives an IC output with glitch-free CMOS transitions and minimal gate depth on the critical path. |
| **Active-LOW /CLR** | Industry-standard fail-safe default. Power interruption resets the FSM to S0 (Idle) without a dedicated reset pulse generator. |
| **74HC over 74LS** | HC CMOS is 3.3V/5V compatible, consumes significantly less static power, and directly interfaces with ESP8266 GPIOs without level-shifters. |
| **Dual 74HC74 Package** | One 14-DIP package contains two independent D-FFs. Two ICs provide all four state bits Q0-Q3 with minimal board area. |
| **I2C OLED over SPI** | Saves 3 GPIO pins compared to SPI -- essential on NodeMCU which has limited GPIOs for simultaneous FSM driver, display, and relay. |
| **Wi-Fi Soft-AP** | No external router dependency; creates its own access point for standalone field deployment. |
| **EEPROM Reconfiguration** | Target pattern is reprogrammable at runtime via authenticated REST API without reflashing firmware. |

---

## 12. 🌍 SDG Alignment

<div align="center">

| SDG Goal | Project Contribution |
|:---:|:---|
| **🏗️ SDG 9**<br>Industry, Innovation & Infrastructure | Demonstrates a modular, reconfigurable hardware FSM topology that can be re-targeted for industrial assembly lines, access control systems, and digital frame synchronization applications without discarding silicon. |
| **♻️ SDG 12**<br>Responsible Consumption & Production | Minimizes electronic waste by enabling dynamic runtime EEPROM reconfiguration, multi-target testbench reuse across 4 different code patterns, and a complete design built from standard off-the-shelf TTL/CMOS 7400-series ICs that remain in production for decades. |

</div>

---

## 13. 🎥 Demo and Media

| File | Description |
|:---|:---|
| 📹 `Complete_Implementation.mp4` | End-to-end system demo: hardware wiring, unlock flow, dashboard, lockout |
| 🖥️ `FSM_SIM_dashboard.mp4` | Browser-based supervisor dashboard walkthrough and code injection demo |
| 🔌 `VLAB_implementation_TinkerCad.mp4` | Virtual lab TinkerCad circuit simulation of the 74HC74 FSM |

---

## 14. ⚙️ How It Works -- Step by Step

1. **User input:** User enters a 4-bit binary code in the supervisor dashboard and clicks **Inject**.
2. **Re-arm:** ESP8266 pulls **/CLR HIGH** to arm the flip-flop chain.
3. **Serial clocking:** For each bit, ESP8266 sets the **DATA line** (D6/GPIO12) HIGH/LOW and pulses the **clock** (D5/GPIO14 rising edge).
4. **Logic evaluation:** On each rising edge, the **74HC08 AND gate array** evaluates D0-D3 from current Q values.
5. **Complement:** The **74HC04 inverter** supplies inverted serial bit `~X`.
6. **State latching:** The **74HC74 flip-flops** latch the new state: `S0 -> S1 -> S2 -> S3 -> S4`.
7. **Unlock:** At **S4**, Q3 goes HIGH, turning on the Green LED, actuating the relay, and GPIO16 reads HIGH.
8. **Feedback:** OLED displays **UNLOCKED / ACCESS GRANTED**; REST API returns `hardwareResult: UNLOCKED`.
9. **Auto-reset:** After 2.5 seconds, ESP8266 pulses **/CLR LOW** to reset all flip-flops back to S0.

---

## 15. 👥 Team

| Role | Contributor |
|:---|:---|
| 🛠️ Hardware Design and IC Wiring | Dharmik Navadia |
| 📐 FSM Logic and Boolean Derivation | Dharmik Navadia |
| 🔌 ESP8266 Firmware (Arduino C++) | Dharmik Navadia |
| 💻 Supervisor Dashboard (HTML/JS) | Dharmik Navadia |
| 🐍 Mock Server and Test Suite | Dharmik Navadia |
| 📖 Documentation and SDG Analysis | Dharmik Navadia |

---

## 16. 📜 License

Distributed under the **MIT License**. See `LICENSE` for full terms.

---

<div align="center">

**Digital System Design (DSD) Micro Project**  
*Bridging 74HC-series CMOS silicon with modern IoT supervisory architecture*  
*74HC74 + 74HC08 + 74HC04 + ESP8266 NodeMCU + SH1106 OLED*

</div>
