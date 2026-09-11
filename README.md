# 🚀 Smart FSM Access Terminal

### ⚡ DSD Micro Project — Hybrid Synchronous Finite State Machine Sequence Detector

<div align="center">

![Architecture](https://img.shields.io/badge/Architecture-Synchronous%20FSM-2563EB?style=for-the-badge)
![ICs](https://img.shields.io/badge/ICs-74HC74%20%7C%2074HC08%20%7C%2074HC04-EA580C?style=for-the-badge)
![MCU](https://img.shields.io/badge/MCU-ESP8266%20NodeMCU-DC2626?style=for-the-badge)
![Firmware](https://img.shields.io/badge/Firmware-Arduino%20C%2B%2B-00979D?style=for-the-badge)
![Server](https://img.shields.io/badge/Mock%20Server-Python%203-3776AB?style=for-the-badge)
![Telemetry](https://img.shields.io/badge/Alerts-Web3Forms%20API-0284C7?style=for-the-badge)
![BOM](https://img.shields.io/badge/BOM%20Cost-Rs.788%20~USD%209.50-16A34A?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-6B7280?style=for-the-badge)
![SDG](https://img.shields.io/badge/SDG-9%20and%2012-7C3AED?style=for-the-badge)

</div>

> **📚 Course:** Digital System Design (DSD) Micro Project
> **🔌 IC Stack:** 74HC74 + 74HC08 + 74HC04 + ESP8266 NodeMCU + SH1106 OLED
> **💰 Total BOM:** Rs. 788 | **🎯 Target Sequence:** 1-0-1-1 | **⚖️ License:** MIT

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
10. [🛡️ Cyber-Physical Security & Escalation Subsystems](#10-️-cyber-physical-security--escalation-subsystems)
11. [💡 Key Design Decisions](#11--key-design-decisions)
12. [🌍 SDG Alignment](#12--sdg-alignment)
13. [🎥 Demo and Media](#13--demo-and-media)
14. [⚙️ How It Works — Step by Step](#14-️-how-it-works--step-by-step)
15. [👥 Team](#15--team)
16. [📜 License](#16--license)

---

## 1. 🔍 Project Overview

This project presents an **industrial-grade Hybrid Synchronous Finite State Machine (FSM)** sequence detector built from discrete 7400-series CMOS ICs and supervised by an ESP8266 NodeMCU over a live Wi-Fi REST API. The design bridges foundational D flip-flop and gate theory with modern IoT SCADA supervisory architectures, featuring an Operator Identity Gatekeeper, a Two-Tier Progressive Interlock, and real-time cloud-dispatched intrusion alerts via the Web3Forms API.

| Attribute | Detail |
|:---|:---|
| **📚 Course** | Digital System Design (DSD) Micro Project |
| **🎯 Detect Target** | `1-0-1-1` (synchronous hardware pattern detection) |
| **⚡ Core Logic ICs** | 74HC74 Dual D-FF, 74HC08 Quad AND, 74HC04 Hex Inverter |
| **🧠 Microcontroller** | ESP8266 NodeMCU v3 — Tensilica 32-bit @ 80 MHz, 4 MB Flash |
| **📺 Display** | SSD1306 / SH1106 0.96-inch I2C OLED, 128×64 px |
| **📡 Connectivity** | Wi-Fi Soft Access Point (`192.168.4.1`), REST API on port 80 |
| **🔢 Encoding Scheme** | One-Hot (5 states: S0–S4) |
| **⚙️ FSM Type** | Hybrid Moore/Mealy synchronous, positive-edge-triggered |
| **🛡️ Safety Layers** | Pre-Access Identity Gate, 30s Cooldown, 5-Strike Chassis Lockout, Hardware Challenge, Web3Forms Intrusion Dispatch |
| **💵 BOM Cost** | Rs. 788.00 (approx. USD 9.50) |
| **🌍 SDG Alignment** | SDG 9 (Innovation) + SDG 12 (Responsible Consumption) |

---

## 2. 🏗️ System Architecture

The system is organized into **four hierarchical layers**:

```text
+---------------------------------------------------------+
| LAYER 4 -- Web Supervisor Dashboard (Browser UI)        |
| fsm_supervisor_dashboard.html                            |
| - Pre-Access Operator Identity Gate (Name + 10-digit Tel) |
| - Tab 1: Smart Lock Terminal (Keypad, Strikes, Monitor)  |
| - Tab 2: Lab FSM Simulator (Live Graph, Logic Analyzer)  |
| - Cloud Alert Dispatch: Web3Forms HTTP REST API          |
+----------------------------+----------------------------+
                             | HTTP REST (port 80 / 8080)
+----------------------------v----------------------------+
| LAYER 3 -- ESP8266 NodeMCU Supervisor Controller         |
| REST Endpoints: /inject /reset /status /tamper           |
| SH1106 OLED Display (I2C) + Hardware FSM Driver          |
+--------+------------------+---------+------------------+
         |                  |         |
    CLK (D5)          DATA (D6)     /CLR (D7)
         |                  |         |
+--------v---+    +----------v-+    +--v---------------+
| 74HC74 #1  |    | 74HC04 U3  |    | 74HC74 #2         |
| D-FF U1A:Q0|    | Inverter   |    | D-FF U2A:Q2       |
| D-FF U1B:Q1|    | X -> ~X    |    | D-FF U2B:Q3 (Z)   |
+------+-----+    +------+-----+    +--------+---------+
       |                 |                    |
       +--------+--------+                    |
                |                              |
+---------------v------------------------------v-------+
| LAYER 2 -- 74HC08 Quad AND Gate Array (U4)            |
| Computes: D0 = X.~Q1.~Q2.~Q3                          |
|           D1 = Q0.~X                                  |
|           D2 = Q1.X                                   |
|           D3 = Q2.X                                   |
+--------------------------------------------------------+
                             |
+----------------------------v---------------------------+
| LAYER 1 -- Physical Output                              |
| Red LEDs: Q0, Q1, Q2 state indicators                    |
| Green LED: Q3 = Z = 1 (UNLOCKED)                          |
| Relay / Solenoid actuator on GPIO0 (D3)                   |
+----------------------------------------------------------+
```

---

## 3. 📐 FSM Mathematical Formulation

### 3.1 State Definitions (One-Hot Encoded)

The FSM detects the non-overlapping binary sequence **1-0-1-1** using 5 states:

| State | Semantic Meaning | One-Hot (Q3 Q2 Q1 Q0) | On Input = 0 | On Input = 1 |
|:---:|:---|:---:|:---|:---|
| **S0** | 🛡️ System Armed / Idle Standby | `0 0 0 0` | Stay S0 | Advance to S1 |
| **S1** | 1️⃣ 1st bit matched (`1`) | `0 0 0 1` | Advance to S2 | Stay S1 |
| **S2** | 2️⃣ 2nd bit matched (`1-0`) | `0 0 1 0` | Return to S0 | Advance to S3 |
| **S3** | 3️⃣ 3rd bit matched (`1-0-1`) | `0 1 0 0` | Return to S0 | Advance to S4 |
| **S4** | 🔓 Sequence complete — UNLOCKED (Z=1) | `1 0 0 0` | Return to S0 | Advance to S1 |

### 3.2 Minimized Boolean Equations

Applying Karnaugh map minimization and one-hot excitation logic:

```
D0  =  X . ~Q1 . ~Q2 . ~Q3     -- Next-state for Q0 (S1: first 1 detected)
D1  =  Q0 . ~X                 -- Next-state for Q1 (S2: 0 follows first 1)
D2  =  Q1 . X                  -- Next-state for Q2 (S3: 1 after 1-0)
D3  =  Q2 . X                  -- Next-state for Q3 (S4: UNLOCK - full 1011)

Output  Z  =  Q3               -- Moore output: HIGH only when in state S4
```

---

## 4. 🛠️ Hardware Design

### 4.1 Pin Mapping — ESP8266 NodeMCU v3

| NodeMCU Pin | GPIO | Direction | Connected To | Signal |
|:---:|:---:|:---:|:---|:---|
| D1 | GPIO5 | OUT | SSD1306 OLED SCL | I2C Clock |
| D2 | GPIO4 | OUT | SSD1306 OLED SDA | I2C Data |
| D3 | GPIO0 | OUT | Relay / Buzzer IN | Solenoid trigger |
| D5 | GPIO14 | OUT | 74HC74 U1+U2 CLK pins | Master synchronous clock |
| D6 | GPIO12 | OUT | 74HC04 Pin 1 (1A) | Serial data bit X |
| D7 | GPIO13 | OUT | 74HC74 U1+U2 /CLR bus | Active-LOW master clear |
| D0 | GPIO16 | IN | 74HC74 U2 Q3 (Pin 9) | Unlock sense (Z output) |

---

## 5. 📂 Repository Structure

```text
Smart-FSM-Access-Terminal/
│
├── esp8266_fsm_controller/
│   └── esp8266_fsm_controller.ino     # 🔌 Arduino firmware: REST API, OLED, FSM driver
│
├── fsm_supervisor_dashboard.html      # 💻 Web Dashboard: Gatekeeper, Keypad, FSM Simulator, Web3Forms
├── mock_esp8266_server.py             # 🐍 Python mock server for local browser testing
├── test_fsm_engine.js                 # 🧪 Node.js: Exhaustive FSM matrix test suite
├── dsd_project_documentation.md       # 📖 Full academic documentation & schematics
├── hardware_wiring_guide.md           # 🔌 Breadboard wiring and pin mapping guide
├── launch_smartlock.bat               # ⚡ One-click Windows launcher (mock server + UI)
├── README.md                          # 📄 Project documentation
├── requirements.txt                   # 📋 Dependencies
├── CONTRIBUTING.md                    # 🤝 Contribution guidelines
├── LICENSE                            # ⚖️ MIT License
└── .gitignore                         # 🚫 Exclusion rules (Maven, IDE, media, temp files)
```

---

## 6. 🚀 Getting Started

### 🔹 Option A — Full Hardware Deployment (Real ESP8266)

**1. Clone the repository:**
```bash
git clone https://github.com/navadiadharmik05-hub/Smart-FSM-Access-Terminal.git
cd Smart-FSM-Access-Terminal
```

**2. Flash firmware:**
Open `esp8266_fsm_controller/esp8266_fsm_controller.ino` in Arduino IDE, select **NodeMCU 1.0 (ESP-12E Module)**, and flash.

**3. Connect to Access Point:**
- SSID: `SMART_LOCK_TERMINAL`
- Password: `smartlock1011`

**4. Launch Dashboard:**
Open `fsm_supervisor_dashboard.html` in your browser. Connect to target IP `192.168.4.1`.

### 🔹 Option B — Local Simulation (No Hardware Required)

**1. Start the mock hardware server:**
```bash
python mock_esp8266_server.py
```

**2. Open Dashboard:**
Double-click `launch_smartlock.bat` or open `fsm_supervisor_dashboard.html` directly in Chrome/Edge.

**3.** Complete the Operator Verification Gate and test inputs via the interactive keypad, or switch to the Lab FSM Simulator.

---

## 7. 🌐 REST API Reference

The supervisor exposes standard HTTP REST endpoints:

| Method | Endpoint | Parameters | Description |
|:---:|:---|:---|:---|
| `GET` | `/status` | None | Live hardware state, Q-registers, strikes, uptime |
| `POST` | `/inject` | `?code=1011` | Serially clocks a 4-bit code into the 74HC flip-flop chain |
| `POST` | `/reset` | None | Pulses `/CLR` LOW — resets all flip-flops to S0 |
| `POST` | `/tamper` | `?active=1` or `0` | Triggers hardware chassis tamper isolation |

---

## 8. 🧪 Automated Test Suite

Run the headless Node.js test suite:

```bash
node test_fsm_engine.js
```

The test suite validates the full 16-combination binary matrix (`0000` to `1111`) against the state transitions, ensuring 0% false-positive and 0% false-negative detection rates.

---

## 9. 🛒 Bill of Materials

| Component | Specification | Qty | Unit (Rs.) | Total (Rs.) |
|:---|:---|:---:|:---:|:---:|
| ESP8266 NodeMCU v3 | Tensilica 32-bit @ 80 MHz, Wi-Fi 802.11 b/g/n, 4MB Flash | 1 | 280.00 | 280.00 |
| 74HC74 Dual D-FF | High-Speed CMOS, 14-DIP, 3.3V/5V compatible | 2 | 22.00 | 44.00 |
| 74HC08 Quad AND Gate | 2-Input AND Gate, 14-DIP | 1 | 18.00 | 18.00 |
| 74HC04 Hex Inverter | 6-Channel NOT Gate, 14-DIP | 1 | 15.00 | 15.00 |
| SSD1306 I2C OLED | 0.96-inch Monochrome 128×64 px | 1 | 190.00 | 190.00 |
| 5V Relay Module | Optocoupler Isolated, 10A 250VAC rating | 1 | 65.00 | 65.00 |
| LED Indicators | 3mm Diffused: 3× Red (state), 1× Green (unlock) | 4 | 2.00 | 8.00 |
| Discrete Passives | 330Ω + 10kΩ resistors & decoupling capacitors | 8 | 1.00 | 8.00 |
| Breadboard + Jumpers | 830-point solderless + assorted wire kit | 1 | 160.00 | 160.00 |
| **TOTAL** | | | | **Rs. 788.00 (~USD 9.50)** |

---

## 10. 🛡️ Cyber-Physical Security & Escalation Subsystems

```text
+-------------------------------------------------------------------------------+
|                       OPERATOR IDENTITY GATEWAY                               |
| Required Authentication: Full Name + Exact 10-Digit Mobile / Badge ID         |
+---------------------------------------+---------------------------------------+
                                         |
                                         v
                 +-----------------------------------------------+
                 | NORMAL OPERATION (Strikes 1 to 2)              |
                 | Real-time Audit Trail & Telemetry Logging      |
                 +----------------------+--------------------------+
                                        |
                                        v (3 Failed Attempts)
                 +-----------------------------------------------+
                 | TIER 1 INTERLOCK: 30-Second Cooldown           |
                 | - /CLR Line Pulled LOW (0V / Hardware Reset)   |
                 | - Keypad Disabled for 30 Seconds               |
                 | - Preserves Strike History (Strikes Held at 3) |
                 +----------------------+--------------------------+
                                        |
                                        v (5 Failed Attempts)
                 +-----------------------------------------------+
                 | TIER 2 INTERLOCK: Permanent Chassis Lockout    |
                 | - Grounded Bus: /CLR Held Continuously at 0V   |
                 | - Acoustic Intrusion Siren Alert               |
                 +----------------------+--------------------------+
                                        |
        +-------------------------------+-------------------------------+
        |                                                               |
        v                                                               v
+--------------------------------+              +--------------------------------+
| WEB3FORMS TELEMETRY DISPATCH    |              | HARDWARE VERIFICATION CHALLENGE |
| Asynchronous POST alert sent    |              | Modal prompts administrator:    |
| with Intruder Name, Mobile,     |              | "Which 74-series IC provides    |
| Violation Count & Timestamp     |              | the Hex Inverter?" (Ans: 74HC04)|
+--------------------------------+              +--------------------------------+
```

### 1. Pre-Access Operator Authentication Gate
Before the dashboard exposes any keypad controls or bit manipulation tools, an operator must enter their Full Name, a strictly validated 10-digit Mobile Number, and select their Purpose of Access. Background interaction remains blurred and locked until verified.

### 2. Tier 1 Interlock — 30-Second Cooldown (Strike 3)
Entering 3 incorrect passcodes triggers a soft interlock. The supervisory layer pulls the `/CLR` bus line to 0V (GND), clearing all flip-flop registers, disabling keypad input, and running a 30-second cooldown timer while retaining the strike count.

### 3. Tier 2 Interlock — Permanent Chassis Tamper Lockout (Strike 5)
If an unauthorized user accumulates 5 consecutive failed attempts:
- The `/CLR` line is grounded permanently, completely isolating the 74HC logic engine.
- An audible siren sounds, and the terminal is locked down.

### 4. Cloud Intrusion Dispatch (Web3Forms API Integration)
Upon reaching Strike 5, the dashboard asynchronously transmits an emergency security alert via the Web3Forms API to the administrator's email, containing:
- Operator Name & Mobile Number (captured at initial authentication)
- Exact Timestamp & Access Purpose
- Violation Level: Maximum Threshold Exceeded (5/5 Strikes)
- Hardware Bus Status: `/CLR` Line Grounded (0V)

### 5. Hardware Verification Recovery Challenge
To clear the Tier 2 lockout, the supervisor requires answering a technical digital logic verification question:

> "Which 74-series integrated circuit provides the logic inverter (NOT gate)?"

Selecting **74HC04** disarms the lockout, restores `/CLR` to HIGH (3.3V), resets strikes to 0, and re-arms the terminal.

---

## 11. 💡 Key Design Decisions

| Design Choice | Engineering Rationale |
|:---|:---|
| **One-Hot Encoding** | Eliminates decoder complexity. Each state bit directly drives an IC output with glitch-free CMOS transitions and minimal gate depth on the critical path. |
| **Active-LOW /CLR** | Industry-standard fail-safe default. Power interruption resets the FSM to S0 (Idle) without a dedicated reset pulse generator. |
| **74HC over 74LS** | High-speed CMOS operates natively across 2V–6V, consumes sub-microamp quiescent current, and directly interfaces with ESP8266 3.3V logic levels. |
| **Dual-Tab Architecture** | Separates real-time field operations (Smart Lock Terminal) from diagnostic engineering (Lab FSM Simulator with 5-channel logic analyzer). |
| **Serverless Alerts** | Web3Forms API enables zero-backend direct email alerting straight from client-side supervisory dashboards. |

---

## 12. 🌍 SDG Alignment

| SDG Goal | Project Contribution |
|:---|:---|
| 🏗️ **SDG 9** — Industry, Innovation & Infrastructure | Implements deterministic, zero-latency physical hardware interlocking for industrial automation, high-reliability safety interlocks, and digital frame synchronization without operating system jitter. |
| ♻️ **SDG 12** — Responsible Consumption & Production | Employs low-power CMOS components consuming only ~26.5 mA and standard, long-lifecycle 7400-series silicon that minimizes electronic waste. |

---

## 13. 🎥 Demo and Media

| File | Description |
|:---|:---|
| 📹 `Complete_Implementation.mp4` | Full physical breadboard demonstration with ESP8266 and solenoid actuation |
| 🖥️ `FSM_SIM_dashboard.mp4` | Supervisor dashboard walkthrough: Gatekeeper, Keypad, and Oscilloscope |
| 🔌 `VLAB_implementation_TinkerCad.mp4` | Autodesk Tinkercad virtual lab simulation of the 74HC circuit |

---

## 14. ⚙️ How It Works — Step by Step

1. **Identity Verification** — Operator fills out their Full Name and 10-digit phone number to unlock the terminal.
2. **Keypad Entry** — Operator clicks `0` or `1` on the dashboard to buffer a 4-bit sequence.
3. **Serial Clocking** — The ESP8266 or simulator synchronizes bit assertions on DATA with rising clock edges on CLK.
4. **Excitation & Progression** — The 74HC08 AND gates and 74HC04 NOT gates evaluate next-state equations on each clock transition.
5. **State Progression** — Sequence advances deterministically from S0 → S1 → S2 → S3 → S4.
6. **Unlock Detection** — Reaching S4 drives Q3 (Z) = 1 (HIGH), turning on the green unlock indicator and actuating the solenoid latch.
7. **Strike Escalation** — Invalid sequences increment strikes, activating the 30s cooldown on Strike 3 and permanent lockout with Web3Forms email dispatch on Strike 5.

---

## 15. 👥 Team

| Role | Contributor |
|:---|:---|
| 🛠️ Hardware Design and IC Wiring | Dharmik Navadia |
| 📐 FSM Logic and Boolean Derivation | Dharmik Navadia |
| 🔌 ESP8266 Firmware (Arduino C++) | Dharmik Navadia |
| 💻 Supervisor Dashboard & Web3Forms Telemetry | Dharmik Navadia |
| 🐍 Mock Server and Test Suite | Dharmik Navadia |
| 📖 Documentation and SDG Analysis | Dharmik Navadia |

---

## 16. 📜 License

Distributed under the **MIT License**. See [`LICENSE`](LICENSE) for full terms.

<div align="center">

**Digital System Design (DSD) Micro Project**
*Bridging 74HC-series CMOS silicon with modern IoT supervisory architecture*

`74HC74` + `74HC08` + `74HC04` + `ESP8266 NodeMCU` + `SH1106 OLED`

</div>
