# Tab5 HID — USB Touch Keyboard & Mouse

Turn the **M5Stack Tab5 (ESP32-P4)** into a fully standalone  
🖱️ **Mouse** + ⌨️ **Keyboard** using only the touchscreen.

---

## 🚧 Current Status

✅ USB HID (keyboard + mouse done)  
🔄 BLE HID (working on)  
🔄 Unified system (planified)

---

## ⚡ Features

- 🖱️ Touchpad mouse (smooth + adjustable speed)
- ⌨️ Full on-screen keyboard (AZERTY)
- 🔁 Real-time input processing (~500Hz loop)
- 🧠 Smart input system (anti-spam / state-based)
- 🔋 Battery monitoring + auto sleep
- 🧪 Debug overlay (live internal state)

---

## 🧩 What is this project?

This is an **embedded HID system** designed for:
- portable control devices
- custom interfaces
- low-level input experimentation

Everything runs directly on the device — no external software required.

---

## 📦 Included

- Main `.ino` firmware (USB HID)
- Custom HID logic
- Full UI system (keyboard + mousepad)
- Optional binary (depending on release)

---

## 🏗️ Core Concept

Touch → Input Engine → HID Output (USB)

- `toutch()` = central brain
- `mousepad()` = cursor logic
- `keys()` = keyboard mapping

---

## 🔋 Optimization Focus

- Minimal screen redraw
- Low power consumption
- Fast input loop
- Idle detection (auto shutdown)
