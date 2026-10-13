# Tab5 HID — USB Touch & Physical Keyboard + Dual-Mode Mouse

Turn the **M5Stack Tab5 (ESP32-P4)** into a standalone USB HID input device combining:

- ⌨️ **On-screen touch keyboard**
- ⌨️ **Physical Tab5KB keyboard**
- 🖱️ **Dual-mode touch mouse**
- 🎮 **Joystick-style mouse control**
- ↕️ **Integrated scrollbar**

All powered by a custom USB HID system.

---

## ⚡ Features

- 🖱️ **Dual-mode touchpad mouse**
  - **Normal Mode:** Standard touchpad mouse with direct cursor movement.
  - **Mixed Mode:** Joystick + mouse in one, providing continuous cursor movement with simulated mouse inertia.
- 🎮 Continuous directional input in Mixed Mode, even when the finger remains stationary.
- ⚙️ Adjustable mouse speed.
- ↕️ Integrated scrollbar for vertical scrolling.
- ⌨️ Full on-screen keyboard (AZERTY).
- ⌨️ Physical **M5Stack Tab5KB** keyboard integration.
- 🔀 Unified input handling for physical and virtual keyboards.
- 🔁 Real-time input processing (~500Hz target loop).
- 🧠 Smart input system (anti-spam / state-based).
- 🔌 Custom USB HID keyboard + mouse implementation.
- 🔋 Battery monitoring + automatic shutdown logic.
- 🧪 Debug overlay (live internal state).
- 💡 Connection status feedback through Tab5KB RGB LEDs.

---

## 🧩 What is this project?

This is an **embedded USB HID system** designed for:

- Portable keyboard and mouse control.
- Touchscreen-based input devices.
- Physical and virtual keyboard integration.
- Custom mouse and joystick-style interaction.
- Low-level HID experimentation.
- Standalone embedded interfaces.

Everything runs directly on the Tab5 — no additional PC software or drivers required for standard USB HID operation.

A USB cable is required to connect the Tab5 to the host computer.

---

## 📦 Included

- Original `.ino` firmware (early virtual-keyboard implementation).
- Custom HID logic.
- Touchscreen keyboard and mouse interface.
- Pre-final firmware build.
- Final USB HID firmware build.

The final firmware is distributed as a compiled binary.

---

## 🏗️ Core Concept

**Touchscreen + Physical Tab5KB → Input Engine → USB HID Output**

The input engine combines multiple sources:

- **Virtual Keyboard:** Touchscreen key input.
- **Physical Keyboard:** Tab5KB hardware key events.
- **Normal Mouse:** Direct touchpad-based cursor movement.
- **Mixed Mouse:** Combined joystick and mouse behavior with continuous movement and inertia.
- **Scrollbar:** Dedicated scrolling input.

All input sources are processed through a unified USB HID system.

### Mouse Modes

**🖱️ Normal Mode — Standard Mouse**

- Direct cursor control through touchscreen movement.
- Adjustable movement speed.
- Standard mouse behavior.
- Mouse button and scrolling support.

**🎮 Mixed Mode — Joystick + Mouse**

- Combines touchpad and joystick-style movement.
- Continuous cursor input while maintaining directional control.
- Simulated mouse inertia for smoother movement.
- Designed for alternative cursor navigation.

---

## 🔋 Optimization Focus

- Minimal screen redraw.
- Low power consumption.
- Fast input processing.
- State-based HID reports.
- Reduced unnecessary USB input traffic.
- Battery and charging monitoring.
- Conditional automatic shutdown.
- Stable long-duration operation.

---

## 🔌 Hardware Requirements

- **M5Stack Tab5 (ESP32-P4)**
- **M5Stack Tab5KB** for physical keyboard input.
- USB cable connected to the Tab5 USB-A port for HID operation.
- USB HID-compatible host computer.

The firmware includes connection-state monitoring and status indicators.

---

## 🚧 Current Status

- ✅ USB HID Keyboard — Final
- ✅ USB HID Mouse — Final
- ✅ On-screen Touch Keyboard
- ✅ Physical Tab5KB Integration
- ✅ Normal Mouse Mode
- ✅ Mixed Mouse Mode (Joystick + Mouse + Inertia)
- ✅ Scrollbar Integration
- ✅ Battery Monitoring
- ✅ Connection Status Feedback
- ✅ Final Stable USB Firmware
- ❌ BLE HID — Not planned
- ❌ Unified USB + BLE — Not planned

**The USB HID implementation is considered complete.**

Development is focused on a stable, standalone USB solution rather than Bluetooth HID support.

---

## 📜 License

**Proprietary Non-Commercial License**

Free personal use, modification and public forks are permitted.
Commercial use is prohibited without written permission.

See [LICENSE.md](LICENSE.md) for full terms.
