# 🖥️ Tab5 HID Project (USB Full v1)

## 📌 Overview
This project turns the **M5Stack Tab5 (ESP32-P4)** into a full **USB HID device**, combining:

- 🖱️ Mouse (touchpad-based)
- ⌨️ Keyboard (on-screen AZERTY layout)
- ⚡ Optimized embedded UI and power management

The goal is to create a **portable standalone input device** with a custom interface and optimized performance.

---

## 🚀 Features (v1 - Full USB)

### 🖱️ Mouse
- Touch-based mousepad
- Adjustable cursor speed (multi-level)
- Left / Right click buttons
- Smooth movement using delta tracking

### ⌨️ Keyboard
- Full on-screen keyboard (AZERTY)
- Letters (upper/lower)
- Numbers
- Special keys:
  - CTRL, ALT, ALT GR, SHIFT
  - TAB, ENTER, BACKSPACE, DELETE
  - Arrow keys

### 🧠 Input Engine
- Centralized touch processing (`toutch()`)
- Multi-touch support
- Lock system to prevent repeated unwanted inputs
- Clean separation:
  - detection (`keyTouched`)
  - state (`lockKey`)
  - execution (`keys()`)

### 🖥️ UI System
- Built with **M5GFX / M5Unified**
- Partial redraw optimization (only small areas updated)
- Components:
  - Control buttons
  - Keyboard grid (49 keys)
  - Mousepad zone
  - Debug panel
  - System status display

### 🔋 Power Management
- Battery monitoring (level, voltage, current)
- Remaining time estimation
- Auto sleep system:
  - Detects inactivity via current consumption
  - Automatic power off

### 🧪 Debug System
- Toggleable debug overlay
- Displays:
  - Touch states
  - Mouse coordinates
  - Internal states

---

## 🏗️ Architecture

loop()
- M5.update()
- toutch() → Main input processing
- state() → UI + system info (every 1s)
- power control → autosleep


### Core Modules

| Module        | Role |
|--------------|------|
| `toutch()`   | Input engine (touch → actions) |
| `mousepad()` | Mouse tracking and movement |
| `keys()`     | Keyboard HID mapping |
| `buttons()`  | UI button creation |
| `initscreen()` | UI initialization |
| `state()`    | System monitoring |
| `debug()`    | Debug display |

---

## ⚙️ Performance & Optimization

- ⚡ Minimal screen redraw
- 🔁 Fast loop (~500Hz with `delay(2)`)
- 🧠 State-based logic (avoids redundant HID calls)
- 🔋 Power-aware behavior
- 📉 Reduced IO when idle

---

## 📦 Included

- `.ino` source file (main logic)
- Custom library folder (if required)
- Precompiled binary (optional)

---

## 🔧 Dependencies

- Arduino IDE
- M5Stack libraries:
  - `M5Unified`
  - `M5GFX`
- USB HID:
  - `USB.h`
  - Custom HID layer (`Tab5USBHID`)

---

## 📦 Hardware

- M5Stack Tab5 (ESP32-P4)
- Integrated touchscreen
- Battery (used for monitoring)

---

## 🛣️ Roadmap

### 🔜 Planned
- BLE HID (keyboard + mouse)
- Unified USB + BLE system
- Config system (profiles, layouts)
- UI improvements

### 🧪 Experimental
- Dual HID instances
- Advanced input modes
- Further power optimization

---

## ⚠️ Notes

- This is **v1 (USB only)**
- BLE support is planned but not included
- Project is optimized for experimentation and performance

---

## 📜 License

This project is licensed under a **Custom Non-Commercial License**.

### ✔️ Allowed
- Personal use
- Modification
- Private redistribution

### ❌ Forbidden
- Commercial use
- Selling the project
- Integration into commercial products
- Redistribution for profit

Commercial use requires **explicit permission from the author**.

---

## 💡 Philosophy

This project explores:
- Embedded UI design
- HID abstraction
- Performance vs usability trade-offs
- Low-level input system control

---

## 👀 Preview

- Touch keyboard
- Integrated mousepad
- Real-time system monitoring
- Debug overlay