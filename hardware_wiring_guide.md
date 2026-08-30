# Hardware Circuit Realization & Breadboard Wiring Guide

**Course:** Digital System Design (DSD)  
**Project:** Hybrid Hardware FSM Sequence Detector & Smart Access Terminal  
**Target Hardware:** NodeMCU ESP8266 v3 + 2x 74HC74 Dual D-FF + 74HC08 Quad AND + 74HC04 Hex Inverter + SSD1306 OLED

---

## 1. Integrated Circuit (IC) Pinout Reference

### 74HC74 — Dual D-Type Positive-Edge-Triggered Flip-Flops
```
                 +---+-V-+---+
     /1CLR (Pin 1)| 1      14 | VCC (+3.3V / +5V)
       1D  (Pin 2)| 2      13 | /2CLR (Pin 13)
     1CLK  (Pin 3)| 3      12 | 2D   (Pin 12)
    /1PRE  (Pin 4)| 4      11 | 2CLK (Pin 11)
       1Q  (Pin 5)| 5      10 | /2PRE (Pin 10)
      /1Q  (Pin 6)| 6       9 | 2Q   (Pin 9)
       GND (Pin 7)| 7       8 | /2Q  (Pin 8)
                 +-----------+
```
> **Important:** Tie all unused active-LOW `/PRE` pins (Pins 4, 10) to **VCC (3.3V)**. Tie both `/CLR` pins (Pins 1, 13) to the common `/CLR` bus controlled by NodeMCU Pin D8 (GPIO15).

---

### 74HC08 — Quad 2-Input AND Gates
```
                 +---+-V-+---+
        1A (Pin 1)| 1      14 | VCC (+3.3V / +5V)
        1B (Pin 2)| 2      13 | 4B (Pin 13)
        1Y (Pin 3)| 3      12 | 4A (Pin 12)
        2A (Pin 4)| 4      11 | 4Y (Pin 11)
        2B (Pin 5)| 5      10 | 3B (Pin 10)
        2Y (Pin 6)| 6       9 | 3A (Pin 9)
       GND (Pin 7)| 7       8 | 3Y (Pin 8)
                 +-----------+
```

---

### 74HC04 — Hex Inverters (NOT Gates)
```
                 +---+-V-+---+
        1A (Pin 1)| 1      14 | VCC (+3.3V / +5V)
        1Y (Pin 2)| 2      13 | 6A (Pin 13)
        2A (Pin 3)| 3      12 | 6Y (Pin 12)
        2Y (Pin 4)| 4      11 | 5A (Pin 11)
        3A (Pin 5)| 5      10 | 5Y (Pin 10)
        3Y (Pin 6)| 6       9 | 4A (Pin 9)
       GND (Pin 7)| 7       8 | 4Y (Pin 8)
                 +-----------+
```

---

## 2. Complete Interconnect Table

| From Node / Source | Pin Number | To Node / Destination | Pin Number | Function |
|---|---|---|---|---|
| **ESP8266 D5 (GPIO14)** | Pin D5 | **74HC74 #1 & #2 CLK** | U1#3, U1#11, U2#3, U2#11 | Synchronous Master Clock (`SCLOCK`) |
| **ESP8266 D6 (GPIO12)** | Pin D6 | **74HC04 #1 (1A)** | U3#1 | Serial Data Input ($X$) |
| **ESP8266 D8 (GPIO15)** | Pin D8 | **74HC74 #1 & #2 /CLR** | U1#1, U1#13, U2#1, U2#13 | Active-LOW Master Clear Bus (`/CLR`) |
| **ESP8266 D0 (GPIO16)** | Pin D0 | **74HC74 #2 Pin 9 (2Q / Q3)** | U2#9 | Unlock Feedback Sensor ($Z$) |
| **ESP8266 D1 (GPIO5)** | Pin D1 | **SSD1306 OLED SCL** | SCL Pin | $I^2C$ Clock |
| **ESP8266 D2 (GPIO4)** | Pin D2 | **SSD1306 OLED SDA** | SDA Pin | $I^2C$ Data |
| **ESP8266 D3 (GPIO0)** | Pin D3 | **5V Relay / Buzzer** | IN Pin | Solenoid / Relay Actuator |
| **74HC04 Pin 2 (1Y / ~X)** | U3#2 | **74HC08 Gate 1 Input (1B)** | U4#2 | Inverted Serial Bit ($\overline{X}$) |
| **74HC74 U1 Pin 5 (Q0)** | U1#5 | **74HC08 Gate 2 Input (2A)** | U4#4 | State Register $Q_0$ (Red LED 0) |
| **74HC74 U1 Pin 9 (Q1)** | U1#9 | **74HC08 Gate 3 Input (3A)** | U4#9 | State Register $Q_1$ (Red LED 1) |
| **74HC74 U2 Pin 5 (Q2)** | U2#5 | **74HC08 Gate 4 Input (4A)** | U4#12 | State Register $Q_2$ (Red LED 2) |
| **74HC74 U2 Pin 9 (Q3)** | U2#9 | **LED Green / Unlock Output** | Anode + Resistor | Output $Z=1$ (Green LED) |

---

## 3. Step-by-Step Breadboard Assembly Instructions

1. **Power Rail Setup:**
   - Connect ESP8266 `3V3` pin to the top red power rail (+) on the breadboard.
   - Connect ESP8266 `GND` pin to the top blue ground rail (-) on the breadboard.
   - Connect `VCC` (Pin 14) of U1, U2, U3, U4 to the 3.3V rail.
   - Connect `GND` (Pin 7) of U1, U2, U3, U4 to the ground rail.

2. **Preset & Clear Inactive Biasing:**
   - Tie Pins 4 and 10 of both 74HC74 ICs (all `/PRE` pins) directly to 3.3V.
   - Connect Pins 1 and 13 of both 74HC74 ICs together into a single `/CLR` bus wire connected to ESP8266 Pin D8.

3. **Clock & Data Distribution:**
   - Run a jumper wire from ESP8266 Pin D5 to Pin 3 of U1, then daisy-chain to Pin 11 of U1, Pin 3 of U2, and Pin 11 of U2.
   - Run a jumper wire from ESP8266 Pin D6 to Pin 1 of the 74HC04 inverter.

4. **Visual LED Indicators:**
   - Connect a $330\Omega$ resistor from each state output ($Q_0, Q_1, Q_2, Q_3$) to the anode of an LED, with the cathode connected to GND.
   - Use Red LEDs for $Q_0, Q_1, Q_2$ and a bright Green LED for $Q_3$.

---

## 4. Hardware Verification & Checklist

- [ ] **Power Check:** Verify $3.30\text{V} \pm 0.1\text{V}$ across Pin 14 and Pin 7 of all ICs using a digital multimeter.
- [ ] **Master Clear Verification:** When `/CLR` is pulsed LOW, all LEDs ($Q_0..Q_3$) must immediately extinguish.
- [ ] **Sequence Test:** Injecting `1-0-1-1` sequentially illuminates $Q_0 \to Q_1 \to Q_2 \to Q_3$ (Green LED ON).
- [ ] **Mismatch Fallback:** Injecting an invalid bit immediately shifts state back to $S_1$ (if bit was `1`) or $S_0$ (if bit was `0`).
