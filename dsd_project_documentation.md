# Academic Technical Report: Hybrid Hardware FSM Sequence Detector & Supervisory Station

**Course:** Digital System Design (DSD)  
**Project Title:** Design and Hardware Realization of a Reconfigurable FSM Sequence Detector for Industrial Safety Interlocks and Digital Frame Synchronization  
**Target Architecture:** 74HC74 Dual D-Type Flip-Flops, 74HC08 Quad AND Gates, 74HC04 Hex Inverters, ESP8266 NodeMCU, SSD1306 I2C OLED

---

## 1. Executive Summary & Syllabus Mapping

This project presents a pedagogical yet industrial-grade **Hybrid Synchronous Finite State Machine (FSM)** sequence detector engineered for high-integrity safety interlocks and digital frame synchronization. The design directly bridges foundational gate/latch theory with modern IoT SCADA supervisory architectures.

### Syllabus Schema Alignment:
- **Module 1.1:** 74HC74 Dual Positive-Edge Triggered D-Type Flip-Flops with Asynchronous Active-LOW Preset and Clear.
- **Module 1.2:** Combinational Next-State Logic Realization via 74HC08 (AND) and 74HC04 (NOT) Gates.
- **Module 3:** Synchronous State Machine Design (Mealy/Moore hybrid), Excitation Truth Tables, State Minimization, and One-Hot Encoding.
- **Module 4:** Active-LOW Interfacing, Multi-strike Progressive Lockout Penalties, Physical Enclosure Tamper Sensors, and Digital Timing Analysis.
- **UN SDGs:** SDG 9 (Industry, Innovation & Infrastructure) & SDG 12 (Responsible Resource Utilization via Modular Reconfiguration).

---

## 2. Mathematical State Machine Formulation

### 2.1 State Definitions (One-Hot Encoded)
For non-overlapping detection of the primary target sequence `1-0-1-1`:

| State | Semantic Meaning | One-Hot Vector $(Q_3 Q_2 Q_1 Q_0)$ |
|:---:|:---|:---:|
| **$S_0$** | System Armed / Idle Standby | `0 0 0 0` |
| **$S_1$** | First Bit Detected (`1`) | `0 0 0 1` |
| **$S_2$** | Second Bit Detected (`1-0`) | `0 0 1 0` |
| **$S_3$** | Third Bit Detected (`1-0-1`) | `0 1 0 0` |
| **$S_4$** | Complete Target Matched (`1-0-1-1`) / Unlocked | `1 0 0 0` |

---

### 2.2 Excitation & State Transition Truth Table

$$\begin{array}{|c|cccc|c|c|cccc|c|c|}
\hline
\text{PS} & Q_3 & Q_2 & Q_1 & Q_0 & \text{Input } X & \text{NS} & Q_3^+ & Q_2^+ & Q_1^+ & Q_0^+ & \text{/CLR} & \text{Output } Z \\
\hline
S_0 & 0 & 0 & 0 & 0 & 0 & S_0 & 0 & 0 & 0 & 0 & 1 & 0 \\
S_0 & 0 & 0 & 0 & 0 & 1 & S_1 & 0 & 0 & 0 & 1 & 1 & 0 \\
\hline
S_1 & 0 & 0 & 0 & 1 & 0 & S_2 & 0 & 0 & 1 & 0 & 1 & 0 \\
S_1 & 0 & 0 & 0 & 1 & 1 & S_1 & 0 & 0 & 0 & 1 & 1 & 0 \\
\hline
S_2 & 0 & 0 & 1 & 0 & 0 & S_0 & 0 & 0 & 0 & 0 & 1 & 0 \\
S_2 & 0 & 0 & 1 & 0 & 1 & S_3 & 0 & 1 & 0 & 0 & 1 & 0 \\
\hline
S_3 & 0 & 1 & 0 & 0 & 0 & S_0 & 0 & 0 & 0 & 0 & 1 & 0 \\
S_3 & 0 & 1 & 0 & 0 & 1 & S_4 & 1 & 0 & 0 & 0 & 1 & 1 \\
\hline
S_4 & 1 & 0 & 0 & 0 & 0 & S_0 & 0 & 0 & 0 & 0 & 1 & 1 \\
S_4 & 1 & 0 & 0 & 0 & 1 & S_1 & 0 & 0 & 0 & 1 & 1 & 1 \\
\hline
\end{array}$$

---

### 2.3 Boolean Derivation for Next-State Flip-Flop Inputs ($D_3 \dots D_0$)

Applying one-hot excitation logic yields the following minimal gate equations:

$$D_0 = X \cdot \overline{Q_1} \cdot \overline{Q_2} \cdot \overline{Q_3}$$

$$D_1 = Q_0 \cdot \overline{X}$$

$$D_2 = Q_1 \cdot X$$

$$D_3 = Q_2 \cdot X$$

$$\text{Output } Z = Q_3$$

---

## 3. Hardware Interconnect & Pinout Mapping

```
                 +---------------------------------------+
                 |          ESP8266 NodeMCU v3           |
                 |                                       |
                 |  D5 (GPIO14) -------> SCLOCK (CLK)    |
                 |  D6 (GPIO12) -------> SDATA (X)       |
                 |  D8 (GPIO15) -------> /CLR_BUS        |
                 |  D0 (GPIO16) <------- Z_SENSE (Q3)    |
                 |  D7 (GPIO13) <------- TAMPER_SWITCH   |
                 |  D1/D2       -------> I2C SSD1306     |
                 +---------------------------------------+
                                     |
               +---------------------+---------------------+
               |                                           |
               v                                           v
    +--------------------+                       +--------------------+
    | 74HC74 Dual D-FF#1 |                       | 74HC74 Dual D-FF#2 |
    | U1A: Q0 (Bit 1)    |                       | U2A: Q2 (Bit 3)    |
    | U1B: Q1 (Bit 2)    |                       | U2B: Q3 (Z Out)    |
    | /CLR Pin 1, 13     |                       | /CLR Pin 1, 13     |
    +--------------------+                       +--------------------+
```

### Complete IC Pin Connection Table:

| IC Part Number | Pin Number | Signal Name | Connected To | Functional Description |
|:---|:---:|:---|:---|:---|
| **74HC74 #1 (U1)** | Pin 1, 13 | $/1\overline{\text{CLR}}, /2\overline{\text{CLR}}$ | ESP8266 D8 (GPIO15) | Active-LOW Master Asynchronous Clear |
| **74HC74 #1 (U1)** | Pin 2 | $1D$ ($D_0$) | 74HC08 Output ($D_0$) | Next-state input for $Q_0$ |
| **74HC74 #1 (U1)** | Pin 3, 11 | $1\text{CLK}, 2\text{CLK}$ | ESP8266 D5 (GPIO14) | Synchronous clock pulse line |
| **74HC74 #1 (U1)** | Pin 5 | $1Q$ ($Q_0$) | 74HC08 Input / LED 0 | Present state $Q_0$ (Red LED) |
| **74HC74 #1 (U1)** | Pin 9 | $2Q$ ($Q_1$) | 74HC08 Input / LED 1 | Present state $Q_1$ (Red LED) |
| **74HC74 #2 (U2)** | Pin 5 | $1Q$ ($Q_2$) | 74HC08 Input / LED 2 | Present state $Q_2$ (Red LED) |
| **74HC74 #2 (U2)** | Pin 9 | $2Q$ ($Q_3$) | ESP8266 D0 / Green LED | Unlock Output signal $Z=1$ |
| **74HC04 (U3)** | Pin 1, 2 | $1A \to 1Y$ | SDATA $\to \overline{X}$ | Inverted Serial Bit stream |
| **74HC08 (U4)** | Pins 1..14 | Quad 2-Input AND | Interconnect array | Computes $D_0, D_1, D_2, D_3$ |
| **SSD1306 OLED** | SDA, SCL | $I^2C$ Bus | ESP8266 D2, D1 | Real-time screen telemetry |

---

## 4. Multi-Layer Industrial Safety Interlocks

1. **Progressive Lockout Scaling:**
   - **Strike 1 & 2:** Warning logged; attempts decremented.
   - **Strike 3:** Progressive lockout penalty ($30\text{s} \to 60\text{s} \to 120\text{s}$).
   - **Hardware Action:** The $/CLR$ bus is forced to $0\text{V}$ (GND), instantly discharging the internal master-slave latches and completely ignoring clock pulses until timer expiration.

2. **Chassis Tamper Isolation (GPIO13):**
   - A physical microswitch wired to ground triggers an immediate non-maskable interrupt (NMI).
   - The ESP8266 pulls $/CLR$ LOW and sounds a continuous acoustic alarm to prevent logic analyzer probing.

3. **Duress Silent Panic Protocol (`1110`):**
   - Submitting the duress sequence unlocks the interlock but silently dispatches an encrypted incident packet to the remote supervisor console without alerting the intruder.

4. **Dynamic 60s Single-Use OTP:**
   - Provides time-limited authentication for temporary field technicians without exposing the permanent master passcode.

---

## 5. IoT REST API Interface Specification

| HTTP Method | Endpoint | Query / Payload | Response Schema | Description |
|:---|:---|:---|:---|:---|
| `POST` | `/inject` | `code=1011` | `{"status":"UNLOCKED","state":"S4","strikes":0,"lockout":false}` | Injects 4-bit sequence into hardware registers |
| `POST` | `/reset` | *None* | `{"status":"RESET_OK","clr":"HIGH"}` | Emits hardware $/CLR$ pulse |
| `POST` | `/reconfigure` | `key=ADMIN9921&newCode=1100` | `{"status":"RECONFIG_OK","newTarget":"1100"}` | Reprograms EEPROM target pattern |
| `POST` | `/test` | *None* | `{"testbench":"16_COMBINATIONS","passed":16,"total":16,"coverage":100}` | Runs complete automated testbench |
| `GET` | `/status` | *None* | `{"target":"1011","strikes":0,"isLocked":false,"freeHeap":41200}` | Queries live hardware telemetry |

---

## 6. Bill of Materials (BOM) & Budget

| Component | Specification | Quantity | Estimated Unit Cost (INR) | Total Cost (INR) |
|:---|:---|:---:|:---:|:---:|
| **ESP8266 NodeMCU v3** | Tensilica 32-bit, 80MHz, Wi-Fi | 1 | ₹280.00 | ₹280.00 |
| **74HC74 Dual D-FF** | High-Speed CMOS 14-DIP | 2 | ₹22.00 | ₹44.00 |
| **74HC08 Quad AND** | 2-Input AND Gate 14-DIP | 1 | ₹18.00 | ₹18.00 |
| **74HC04 Hex Inverter** | 6-Channel Inverter 14-DIP | 1 | ₹15.00 | ₹15.00 |
| **SSD1306 I2C OLED** | 0.96-inch Monochrome 128x64 | 1 | ₹190.00 | ₹190.00 |
| **5V Relay Module** | Optocoupler Isolated 10A 250V | 1 | ₹65.00 | ₹65.00 |
| **LED Indicators** | 3mm Diffused (3x Red, 1x Green) | 4 | ₹2.00 | ₹8.00 |
| **Discrete Passives** | 330Ω, 10kΩ Resistors & Decoupling Caps | 8 | ₹1.00 | ₹8.00 |
| **Breadboard & Jumpers** | 830-Point Solderless + Wires | 1 | ₹160.00 | ₹160.00 |
| **Total Project Cost** | | | | **₹788.00 (~$9.50 USD)** |

---

## 7. Sustainable Development Goals (SDG) Alignment

- **SDG 9: Industry, Innovation & Infrastructure:** Demonstrates modular, reconfigurable hardware topologies that can be re-purposed dynamically across industrial assembly lines without discarding legacy silicon.
- **SDG 12: Responsible Consumption and Production:** Minimizes electronic waste (e-Waste) by implementing dynamic EEPROM reconfiguration and multi-use testbenches on standard off-the-shelf TTL/CMOS integrated circuits.
