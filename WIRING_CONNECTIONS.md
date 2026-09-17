# ðŸ  Smart Home â€” Complete Wiring & Connection Guide

> **Hardware:** ESP32-C3 SuperMini (Ã—2 for Sinric Pro boards, Ã—1 for RainMaker board)  
> **Relay Module:** Active-LOW (energized when GPIO = LOW)  
> **Switches:** Momentary / Toggle â€” connected with **INPUT_PULLUP** (one side to GPIO, other side to GND)

---

## ðŸ“¦ Board Overview

| Board | Firmware | Integration | Channels |
|-------|----------|-------------|----------|
| **Board 1** | `smart_home_c3_board1.ino` | Sinric Pro (Account 1) | Fan 1, Fan 2, LED Tube |
| **Board 2** | `smart_home_c3_board2.ino` | Sinric Pro (Account 2) | LED Bulb, Night Light, Speaker |
| **Board C3** | `smart_home_c3.ino` | ESP RainMaker + Google Home | All 6 channels (single board) |

---

## ðŸ”Œ Board 1 â€” Sinric Pro (Channels 1â€“3)

### Pin Connections

| Device | Relay GPIO | Relay Wire | Wall Switch GPIO | Switch Wire |
|--------|-----------|------------|-----------------|-------------|
| **Bedroom Fan 1** | `GPIO 0` | Relay IN1 | `GPIO 7` | Switch terminal |
| **Bedroom Fan 2** | `GPIO 1` | Relay IN2 | `GPIO 10` | Switch terminal |
| **LED Tube Light** | `GPIO 3` | Relay IN3 | `GPIO 20` | Switch terminal |

### Relay Module Wiring (per channel)
```
ESP32-C3          Relay Module
â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€        â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
GPIO 0   â”€â”€â”€â”€â”€â”€â”€â–º IN1  (Fan 1 control)
GPIO 1   â”€â”€â”€â”€â”€â”€â”€â–º IN2  (Fan 2 control)
GPIO 3   â”€â”€â”€â”€â”€â”€â”€â–º IN3  (LED Tube control)
3.3V/5V  â”€â”€â”€â”€â”€â”€â”€â–º VCC
GND      â”€â”€â”€â”€â”€â”€â”€â–º GND

Relay Output (NO/COM) connects to AC Load:
  COM  â”€â”€â”€â”€ Live wire (AC In)
  NO   â”€â”€â”€â”€ Load (AC Out to appliance)
```

### Wall Switch Wiring (per channel)
```
ESP32-C3          Physical Switch
â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€        â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
GPIO 7   â”€â”€â”€â”€â”€â”€â”€â–º Terminal 1  (Fan 1 switch)
GPIO 10  â”€â”€â”€â”€â”€â”€â”€â–º Terminal 1  (Fan 2 switch)
GPIO 20  â”€â”€â”€â”€â”€â”€â”€â–º Terminal 1  (LED Tube switch)
GND      â”€â”€â”€â”€â”€â”€â”€â–º Terminal 2  (ALL switches share GND)

INPUT_PULLUP is used â€” switch shorts GPIO to GND when pressed.
```

---

## ðŸ”Œ Board 2 â€” Sinric Pro (Channels 4â€“6)

### Pin Connections

| Device | Relay GPIO | Relay Wire | Wall Switch GPIO | Switch Wire |
|--------|-----------|------------|-----------------|-------------|
| **LED Bulb** | `GPIO 4` | Relay IN1 | `GPIO 21` | Switch terminal |
| **Night Light** | `GPIO 5` | Relay IN2 | `GPIO 2` | Switch terminal |
| **Speaker** | `GPIO 6` | Relay IN3 | `GPIO 8` | Switch terminal |

### Relay Module Wiring
```
ESP32-C3          Relay Module
â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€        â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
GPIO 4   â”€â”€â”€â”€â”€â”€â”€â–º IN1  (LED Bulb control)
GPIO 5   â”€â”€â”€â”€â”€â”€â”€â–º IN2  (Night Light control)
GPIO 6   â”€â”€â”€â”€â”€â”€â”€â–º IN3  (Speaker control)
3.3V/5V  â”€â”€â”€â”€â”€â”€â”€â–º VCC
GND      â”€â”€â”€â”€â”€â”€â”€â–º GND
```

### Wall Switch Wiring
```
ESP32-C3          Physical Switch
â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€        â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
GPIO 21  â”€â”€â”€â”€â”€â”€â”€â–º Terminal 1  (LED Bulb switch)
GPIO 2   â”€â”€â”€â”€â”€â”€â”€â–º Terminal 1  (Night Light switch)
GPIO 8   â”€â”€â”€â”€â”€â”€â”€â–º Terminal 1  (Speaker switch)
GND      â”€â”€â”€â”€â”€â”€â”€â–º Terminal 2  (ALL switches share GND)
```

---

## ðŸ”Œ Board C3 â€” RainMaker + Google Home (All 6 Channels)

### Complete Pin Map

| Device | Relay GPIO | Wall Switch GPIO |
|--------|-----------|-----------------|
| **Fan 1** | `GPIO 0` | `GPIO 7` |
| **Fan 2** | `GPIO 1` | `GPIO 10` |
| **LED Tube** | `GPIO 3` | `GPIO 20` |
| **LED Bulb** | `GPIO 4` | `GPIO 21` |
| **Night Lamp** | `GPIO 5` | `GPIO 2` |
| **Speaker Socket** | `GPIO 6` | `GPIO 8` |
| **Reset Button** | â€” | `GPIO 9` (BOOT btn) |

### Full Relay Wiring
```
ESP32-C3          6-Ch Relay Module
â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€        â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
GPIO 0   â”€â”€â”€â”€â”€â”€â”€â–º IN1  (Fan 1)
GPIO 1   â”€â”€â”€â”€â”€â”€â”€â–º IN2  (Fan 2)
GPIO 3   â”€â”€â”€â”€â”€â”€â”€â–º IN3  (LED Tube)
GPIO 4   â”€â”€â”€â”€â”€â”€â”€â–º IN4  (LED Bulb)
GPIO 5   â”€â”€â”€â”€â”€â”€â”€â–º IN5  (Night Lamp)
GPIO 6   â”€â”€â”€â”€â”€â”€â”€â–º IN6  (Speaker Socket)
3.3V/5V  â”€â”€â”€â”€â”€â”€â”€â–º VCC
GND      â”€â”€â”€â”€â”€â”€â”€â–º GND
```

### Full Wall Switch Wiring
```
ESP32-C3          Physical Switches
â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€        â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
GPIO 7   â”€â”€â”€â”€â”€â”€â”€â–º Switch 1 (Fan 1)
GPIO 10  â”€â”€â”€â”€â”€â”€â”€â–º Switch 2 (Fan 2)
GPIO 20  â”€â”€â”€â”€â”€â”€â”€â–º Switch 3 (LED Tube)
GPIO 21  â”€â”€â”€â”€â”€â”€â”€â–º Switch 4 (LED Bulb)
GPIO 2   â”€â”€â”€â”€â”€â”€â”€â–º Switch 5 (Night Lamp)
GPIO 8   â”€â”€â”€â”€â”€â”€â”€â–º Switch 6 (Speaker Socket)
GND      â”€â”€â”€â”€â”€â”€â”€â–º Common terminal of ALL switches
```

### Reset Button (GPIO 9)
```
GPIO 9 â”€â”€â”€â”€ BOOT button (built-in on ESP32-C3 SuperMini)
GND    â”€â”€â”€â”€ Other side (already on-board)

Hold Duration:
  > 3 seconds  â†’ Wi-Fi credentials reset
  > 10 seconds â†’ Full factory reset
```

---

## Power Supply

```
220V AC Mains
â”‚
â”œâ”€â”€â–º 5V DC Adapter â”€â”€â–º ESP32-C3 5V pin (or USB)
â”‚                  â””â”€â”€â–º Relay Module VCC
â”‚
â””â”€â”€â–º AC Live wire â”€â”€â–º Relay COM terminals (switched load)
     AC Neutral  â”€â”€â–º Load Neutral (direct)
```

---

## Switch Behavior

- Switches use INPUT_PULLUP â€” one terminal to GPIO, other terminal to GND
- Any state change (press/release) toggles the relay â€” works like a smart toggle switch
- 200ms debounce prevents false triggers
- State is saved to NVS (flash) and restored on reboot

---

## Cloud Integration Summary

| Board | Platform | App | Google Home |
|-------|----------|-----|-------------|
| Board 1 | Sinric Pro | Sinric Pro App | via Sinric |
| Board 2 | Sinric Pro | Sinric Pro App | via Sinric |
| Board C3 | ESP RainMaker | ESP RainMaker App | Native |

