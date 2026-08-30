#!/usr/bin/env python3
"""
==============================================================================================
Smart Access Terminal: Mock ESP8266 REST API Server for Local Hardware Testing
Course: Digital System Design (DSD)

Description:
  Mock hardware server on http://127.0.0.1:8080:
    - GET  /status
    - POST /inject?code={bitstream}
    - POST /tamper?active=1|0
    - POST /reset
==============================================================================================
"""

from http.server import HTTPServer, BaseHTTPRequestHandler
import urllib.parse
import json
import time

HOST = "127.0.0.1"
PORT = 8080

state = {
    "target": "1011",
    "currentState": "S0",
    "flipFlops": [0, 0, 0, 0], # [Q3, Q2, Q1, Q0]
    "outputZ": 0,
    "strikes": 0,
    "progressiveMultiplier": 1,
    "isLocked": False,
    "isTampered": False,
    "lockoutUntil": 0,
    "clrLine": 1, # 1: High (Normal), 0: Low (Reset/Lockout)
    "bootTime": time.time()
}

class SmartLockAPIHandler(BaseHTTPRequestHandler):
    def _send_cors(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type, Authorization")

    def do_OPTIONS(self):
        self.send_response(204)
        self._send_cors()
        self.end_headers()

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        if path == "/" or path == "/status":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self._send_cors()
            self.end_headers()

            if state["isLocked"] and time.time() >= state["lockoutUntil"]:
                state["isLocked"] = False
                state["strikes"] = 0
                state["clrLine"] = 1 if not state["isTampered"] else 0
                state["currentState"] = "S0"
                state["flipFlops"] = [0, 0, 0, 0]
                print("[HARDWARE] Lockout penalty elapsed. System re-armed.")

            payload = {
                "status": "ONLINE",
                "firmware": "SmartLock_v3.5",
                "target": "1011",
                "currentState": state["currentState"],
                "flipFlops": state["flipFlops"],
                "hardwareQ3Sense": state["outputZ"],
                "strikeCount": state["strikes"],
                "isLockedOut": state["isLocked"],
                "isTampered": state["isTampered"],
                "lockoutRemainingSec": max(0, int(state["lockoutUntil"] - time.time())) if state["isLocked"] else 0,
                "clrLine": state["clrLine"],
                "uptimeSeconds": int(time.time() - state["bootTime"])
            }
            self.wfile.write(json.dumps(payload, indent=2).encode("utf-8"))
        else:
            self.send_response(404)
            self.end_headers()

    def do_POST(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path
        query = urllib.parse.parse_qs(parsed.query)

        content_len = int(self.headers.get('Content-Length', 0))
        body_str = self.rfile.read(content_len).decode('utf-8') if content_len > 0 else ""
        if body_str and not query:
            query = urllib.parse.parse_qs(body_str)

        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self._send_cors()
        self.end_headers()

        response = {}

        if path == "/inject":
            code = query.get("code", [""])[0]
            if len(code) != 4 or not all(c in "01" for c in code):
                response = {"error": "INVALID_CODE", "message": "Code must be 4 binary bits"}
            elif state["isLocked"] or state["isTampered"]:
                response = {
                    "hardwareResult": "LOCKED_OUT",
                    "hardwareQ3Sense": 0,
                    "strikeCount": state["strikes"],
                    "isLockedOut": state["isLocked"],
                    "isTampered": state["isTampered"]
                }
            else:
                print(f"[HARDWARE] Keypad Injected 4-Bit Stream: {code}")
                s = 0
                target = "1011"
                for bit in code:
                    b = int(bit)
                    if b == int(target[s]):
                        s += 1
                    else:
                        s = 1 if b == 1 else 0

                state["currentState"] = f"S{s}"
                if s == 1: state["flipFlops"] = [0, 0, 0, 1]
                elif s == 2: state["flipFlops"] = [0, 0, 1, 0]
                elif s == 3: state["flipFlops"] = [0, 1, 0, 0]
                elif s == 4: state["flipFlops"] = [1, 0, 0, 0]
                else: state["flipFlops"] = [0, 0, 0, 0]

                is_unlocked = (s == 4)
                state["outputZ"] = 1 if is_unlocked else 0

                if is_unlocked:
                    state["strikes"] = 0
                    state["progressiveMultiplier"] = 1
                    response = {
                        "hardwareResult": "UNLOCKED",
                        "hardwareQ3Sense": 1,
                        "strikeCount": 0,
                        "isLockedOut": False,
                        "lockoutRemainingSec": 0
                    }
                    print(f"[HARDWARE] ✓ ACCESS GRANTED: Solenoid Unlocked (Q3=1).")
                else:
                    state["strikes"] += 1
                    if state["strikes"] >= 3:
                        state["isLocked"] = True
                        penalty = 30 * state["progressiveMultiplier"]
                        state["lockoutUntil"] = time.time() + penalty
                        state["clrLine"] = 0
                        response = {
                            "hardwareResult": "DENIED",
                            "hardwareQ3Sense": 0,
                            "strikeCount": state["strikes"],
                            "isLockedOut": True,
                            "lockoutRemainingSec": penalty
                        }
                        print(f"[HARDWARE] ✗ 3 STRIKES: System Locked for {penalty}s (/CLR=GND).")
                        state["progressiveMultiplier"] = min(4, state["progressiveMultiplier"] * 2)
                    else:
                        response = {
                            "hardwareResult": "DENIED",
                            "hardwareQ3Sense": 0,
                            "strikeCount": state["strikes"],
                            "isLockedOut": False,
                            "lockoutRemainingSec": 0
                        }
                        print(f"[HARDWARE] ✗ ACCESS DENIED: Strike {state['strikes']}/3.")

        elif path == "/tamper":
            active = query.get("active", ["0"])[0] == "1"
            state["isTampered"] = active
            state["clrLine"] = 0 if active else 1
            if active:
                state["currentState"] = "S0"
                state["flipFlops"] = [0, 0, 0, 0]
                state["outputZ"] = 0
                print("🚨 [HARDWARE] CHASSIS TAMPER DETECTED (GPIO13 LOW). /CLR Held at GND.")
            else:
                print("🛡 [HARDWARE] Chassis safety restored. /CLR Re-armed HIGH.")
            response = {"status": "TAMPER_UPDATED", "isTampered": active}

        elif path == "/reset":
            state["currentState"] = "S0"
            state["flipFlops"] = [0, 0, 0, 0]
            state["outputZ"] = 0
            state["strikes"] = 0
            state["isLocked"] = False
            state["isTampered"] = False
            state["clrLine"] = 1
            response = {"status": "RESET_OK", "clr": "HIGH", "state": "S0"}
            print("[HARDWARE] ⚡ Force Hardware /CLR Pulse Executed.")

        else:
            response = {"status": "OK"}

        self.wfile.write(json.dumps(response, indent=2).encode("utf-8"))

    def log_message(self, format, *args):
        return

def run():
    server_address = (HOST, PORT)
    httpd = HTTPServer(server_address, SmartLockAPIHandler)
    print("=" * 70)
    print(f"  ⚡ SMART ACCESS TERMINAL — MOCK HARDWARE SERVER (http://{HOST}:{PORT})")
    print("=" * 70)
    print("  Endpoints: GET /status | POST /inject | POST /tamper | POST /reset")
    print("  Press Ctrl+C to stop.")
    print("=" * 70)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n[HARDWARE] Server stopped.")
        httpd.server_close()

if __name__ == "__main__":
    run()
