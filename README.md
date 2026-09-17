# 🏠 My Home — ESP32-C3 Smart Home System

<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32--C3-blue?logo=espressif" />
  <img src="https://img.shields.io/badge/Cloud-Sinric%20Pro-orange" />
  <img src="https://img.shields.io/badge/Cloud-ESP%20RainMaker-green" />
  <img src="https://img.shields.io/badge/Voice-Google%20Home-yellow?logo=google" />
  <img src="https://img.shields.io/badge/Language-Arduino%20C%2B%2B-informational" />
</p>

A complete **6-channel smart home automation system** built on the **ESP32-C3 SuperMini** microcontroller.  
Control lights, fans, and appliances via **voice (Google Home)**, **smartphone app**, or **physical wall switches** — all three work simultaneously and stay in sync.

---

## 📁 Project Structure

```
my-home/
│
├── README.md                          ← You are here
├── WIRING_CONNECTIONS.md              ← Full hardware wiring guide
├── .gitignore
│
├── smart_home_c3_board1/              ← Board 1: Sinric Pro (Channels 1–3)
│   └── smart_home_c3_board1.ino
│
├── smart_home_c3_board2/              ← Board 2: Sinric Pro (Channels 4–6)
│   └── smart_home_c3_board2.ino
│
└── smart_home_c3/                     ← Board C3: ESP RainMaker + Google Home
    └── smart_home_c3.ino
```

---

## 📦 Board Overview

| Board Folder | Chip | Cloud Platform | Devices Controlled |
|---|---|---|---|
| `smart_home_c3_board1/` | ESP32-C3 SuperMini | **Sinric Pro** (Account 1) | Bedroom Fan 1, Bedroom Fan 2, LED Tube Light |
| `smart_home_c3_board2/` | ESP32-C3 SuperMini | **Sinric Pro** (Account 2) | LED Bulb, Night Light, Speaker Socket |
| `smart_home_c3/` | ESP32-C3 SuperMini | **ESP RainMaker** + Google Home | All 6 channels (single board variant) |

---

## ⚡ Features

- ✅ **Voice Control** — Google Home / Alexa via Sinric Pro or ESP RainMaker
- ✅ **App Control** — Sinric Pro App or ESP RainMaker App (iOS & Android)
- ✅ **Physical Wall Switch** — Any toggle/momentary switch works, always in sync with cloud
- ✅ **State Persistence** — Last state saved to flash (NVS), restored on power-up
- ✅ **Debounced Switches** — 200ms debounce prevents false triggers
- ✅ **Wi-Fi Reset** — Hold BOOT button >3s to reset Wi-Fi, >10s for full factory reset
- ✅ **Active-LOW Relay** — Compatible with standard relay modules
- ✅ **Dual Account Support** — Board 1 & Board 2 use separate Sinric Pro accounts (avoids device limits)

---

## 🔌 Pin Mapping — All Boards

### Board 1 & Board C3 Channels 1–3

| Channel | Device | Relay GPIO | Wall Switch GPIO |
|---------|--------|-----------|-----------------|
| 1 | Bedroom Fan 1 | `GPIO 0` | `GPIO 7` |
| 2 | Bedroom Fan 2 | `GPIO 1` | `GPIO 10` |
| 3 | LED Tube Light | `GPIO 3` | `GPIO 20` |

### Board 2 & Board C3 Channels 4–6

| Channel | Device | Relay GPIO | Wall Switch GPIO |
|---------|--------|-----------|-----------------|
| 4 | LED Bulb | `GPIO 4` | `GPIO 21` |
| 5 | Night Light | `GPIO 5` | `GPIO 2` |
| 6 | Speaker Socket | `GPIO 6` | `GPIO 8` |

### Special Pins

| Pin | Function |
|-----|----------|
| `GPIO 9` | BOOT / Reset Button (Board C3 only) |

> 📄 Full wiring diagrams with relay module and AC connections: **[WIRING_CONNECTIONS.md](./WIRING_CONNECTIONS.md)**

---

## 🔧 Hardware Required

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32-C3 SuperMini | 1–2 | 1 for Board C3, or 2 for Board1 + Board2 |
| 3-Channel or 6-Channel Relay Module | 1–2 | Active-LOW, 5V coil |
| Toggle / Momentary Wall Switches | 6 | One per channel |
| 5V DC Power Adapter | 1–2 | Powers ESP32-C3 + Relay module |
| Jumper Wires | As needed | For GPIO to relay IN connections |
| AC-rated wire | As needed | For relay output to appliances |

---

## 🛠️ Software Setup

### Arduino IDE Configuration

1. Install **Arduino IDE 2.x**
2. Add ESP32 board support:
   - Go to **File → Preferences → Additional Board Manager URLs**
   - Add: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Install **esp32 by Espressif Systems** from Board Manager

### For Board 1 & Board 2 (Sinric Pro)

```
Board       : ESP32C3 Dev Module
USB CDC On Boot : Enabled
Partition   : Default 4MB with spiffs
```

Install libraries via **Sketch → Include Library → Manage Libraries**:
- `SinricPro` by Boris Jaeger

### For Board C3 (ESP RainMaker)

```
Board       : ESP32C3 Dev Module  
USB CDC On Boot : Enabled
Partition   : RainMaker 4MB No OTA
```

> ⚠️ The RainMaker partition scheme is mandatory. Without it, the firmware will not compile.

Libraries needed (included with ESP32 Arduino core):
- `RMaker.h`
- `WiFiProv.h`

---

## 🔑 Credentials Setup

### Board 1 — Sinric Pro Account 1
Edit `smart_home_c3_board1.ino`:
```cpp
#define WIFI_SSID    "your_wifi_ssid"
#define WIFI_PASS    "your_wifi_password"
#define APP_KEY      "your_sinric_app_key"
#define APP_SECRET   "your_sinric_app_secret"
#define DEVICE_ID_FAN1    "your_device_id"
#define DEVICE_ID_FAN2    "your_device_id"
#define DEVICE_ID_LEDTUBE "your_device_id"
```

### Board 2 — Sinric Pro Account 2
Edit `smart_home_c3_board2.ino`:
```cpp
#define WIFI_SSID     "your_wifi_ssid"
#define WIFI_PASS     "your_wifi_password"
#define APP_KEY       "your_sinric_app_key"
#define APP_SECRET    "your_sinric_app_secret"
#define DEVICE_ID_LEDBULB "your_device_id"
#define DEVICE_ID_NIGHT   "your_device_id"
#define DEVICE_ID_SPEAKER "your_device_id"
```

### Board C3 — ESP RainMaker
No pre-configured credentials needed.  
Wi-Fi is provisioned via **SoftAP** using the **ESP RainMaker mobile app**.

---

## 📱 First-Time Setup (Board C3 — RainMaker)

1. Flash `smart_home_c3.ino` to the ESP32-C3
2. Open Serial Monitor at **115200 baud**
3. The board will broadcast a Wi-Fi hotspot: `PROV_SmartHome`
4. **On your phone:**
   - Turn OFF mobile data
   - Connect to `PROV_SmartHome`
   - Open **ESP RainMaker App** → Add Device → Scan QR code shown in Serial Monitor
   - Enter your home Wi-Fi credentials
5. Once provisioned, the device appears in the app and links to **Google Home** automatically

---

## 🔁 Wall Switch Behavior

- Switches are wired with `INPUT_PULLUP` — **one terminal to GPIO, other to GND**
- Any **state change** (press or release) toggles the relay
- Works as a smart toggle — pressing once turns ON, pressing again turns OFF
- Cloud state is **automatically synced** after each physical switch action
- 200ms debounce built in

---

## 🔄 Reset Button (Board C3 — GPIO 9)

| Hold Duration | Action |
|--------------|--------|
| < 3 seconds | No action |
| 3 – 10 seconds | Wi-Fi credentials reset (re-provisioning required) |
| > 10 seconds | Full factory reset (all data erased) |

---

## 🔌 Wiring Summary (Quick Reference)

```
Physical Switch Wiring:
  Switch Terminal 1  →  ESP32-C3 GPIO (see pin map above)
  Switch Terminal 2  →  GND

Relay Module Wiring:
  Relay IN1–IN6      →  ESP32-C3 GPIO 0,1,3,4,5,6
  Relay VCC          →  5V
  Relay GND          →  GND

Relay Output (AC):
  COM                →  AC Live (Input)
  NO (Normally Open) →  AC Live to appliance
  Neutral            →  Direct to appliance (not through relay)
```

> ⚠️ **SAFETY:** Always switch off mains power before working on AC wiring. Use properly insulated, rated connectors and wire gauges for your load.

---

## 📡 Cloud Architecture

```
                    ┌─────────────────────────────┐
                    │        Your Home Wi-Fi        │
                    └──────────────┬──────────────┘
                                   │
              ┌────────────────────┼───────────────────┐
              │                    │                   │
     ┌────────▼──────┐   ┌────────▼──────┐   ┌───────▼───────┐
     │   Board 1     │   │   Board 2     │   │   Board C3    │
     │  ESP32-C3     │   │  ESP32-C3     │   │  ESP32-C3     │
     │  Sinric Pro   │   │  Sinric Pro   │   │  RainMaker    │
     │  Account 1    │   │  Account 2    │   │  + Google Home│
     └───────────────┘   └───────────────┘   └───────────────┘
          │                    │                    │
     Fan1, Fan2,          LED Bulb,            All 6 channels
     LED Tube             Night, Speaker        (single board)
```

---

## 📋 File Reference

| File | Description |
|------|-------------|
| [`smart_home_c3_board1.ino`](./smart_home_c3_board1/smart_home_c3_board1.ino) | Board 1 firmware — Sinric Pro, 3 channels |
| [`smart_home_c3_board2.ino`](./smart_home_c3_board2/smart_home_c3_board2.ino) | Board 2 firmware — Sinric Pro, 3 channels |
| [`smart_home_c3.ino`](./smart_home_c3/smart_home_c3.ino) | Board C3 firmware — RainMaker + Google Home, 6 channels |
| [`WIRING_CONNECTIONS.md`](./WIRING_CONNECTIONS.md) | Detailed hardware wiring diagrams |

---

## 👤 Author

**SARMA-Project**  
GitHub: [github.com/SARMA-Project](https://github.com/SARMA-Project)

---

## 📄 License

This project is open source. Feel free to use, modify, and share.
