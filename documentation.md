# 🖥️ Tab5 HID Project (USB Full — Final Release)

## 📌 Overview

This project turns the **M5Stack Tab5 (ESP32-P4)** into a complete **USB HID keyboard and mouse device**, combining:

- 🖱️ Dual-mode touchscreen mouse (Normal + Mixed)
- 🎮 Joystick-style cursor control with mouse inertia
- ↕️ Integrated vertical scrollbar
- ⌨️ On-screen AZERTY keyboard
- ⌨️ Physical M5Stack Tab5KB keyboard integration
- ⚡ Optimized embedded UI, USB HID processing and power management

The goal is to create a **portable standalone input device** with a custom interface, physical and virtual controls, and optimized performance.

No additional software is required on the host computer for standard USB HID operation.

---

## 🚀 Features (Final USB Release)

### 🖱️ Dual-Mode Mouse

**Normal Mode — Standard Mouse**
- Touch-based mousepad
- Direct cursor movement using touch delta tracking
- Adjustable cursor speed (multi-level)
- Left / Right click buttons
- Smooth and responsive movement

**Mixed Mode — Joystick + Mouse**
- Combines standard mouse and joystick-style cursor control
- Continuous cursor movement while directional input is maintained
- Mouse inertia for smoother movement
- Allows both direct touch movement and sustained directional control
- Designed for alternative cursor navigation

**Scrollbar**
- Dedicated vertical scrolling control
- Integrated with USB HID mouse reports
- Available alongside mouse navigation

### ⌨️ Keyboard

**Virtual Keyboard**
- Full on-screen AZERTY keyboard
- 49-key touchscreen layout
- Letters (upper/lower)
- Numbers
- Special keys:
  - CTRL, ALT, ALT GR, SHIFT
  - TAB, ENTER, BACKSPACE, DELETE
  - Arrow keys

**Physical Keyboard — Tab5KB**
- Integrated M5Stack Tab5KB physical keyboard
- Hardware key event processing
- USB HID key forwarding
- Combined physical and virtual keyboard input handling
- RGB LED connection status feedback

### 🧠 Input Engine

- Centralized touchscreen processing (`toutch()`)
- Multi-touch support
- Lock system to prevent repeated unwanted inputs
- State-based input processing
- Combined physical and virtual keyboard events
- Mouse movement and button state management
- HID report handling for keyboard and mouse

The touchscreen input architecture separates:
- Detection (`keyTouched`)
- State (`lockKey`)
- Execution (`keys()`)

### 🖥️ UI System

Built with **M5GFX / M5Unified**.

- Partial redraw optimization
- Touch-responsive interface
- Integrated virtual keyboard and mousepad
- Physical keyboard support
- Mouse mode controls
- Scrollbar
- Battery and system status monitoring
- Debug information

Main interface components:
- Control buttons
- Keyboard grid (49 keys)
- Mousepad zone
- Mouse mode controls
- Scrollbar
- Debug panel
- System status display

### 🔋 Power Management

- Battery level monitoring
- Battery voltage and current monitoring
- Remaining time estimation
- Charging state monitoring
- Reduced unnecessary processing while idle
- Conditional automatic power-off logic

### 🧪 Debug & Status System

- Debug information for input processing
- Touch state monitoring
- Mouse coordinates and movement states
- Internal input states
- USB HID connection status
- Tab5KB RGB LED feedback

---

## 🏗️ Architecture

The final firmware combines touchscreen interaction, physical keyboard events and USB HID output.

**Input sources → Input Engine → HID Reports → USB Host**

Main processing flow:

```text
loop()
 ├── M5.update()
 ├── Tab5KB update
 ├── Physical keyboard event processing
 ├── Touchscreen input processing
 │    ├── Virtual keyboard
 │    ├── Mousepad
 │    ├── Mouse mode handling
 │    └── Scrollbar
 ├── USB HID keyboard/mouse reports
 ├── Battery and connection monitoring
 └── Short loop delay
```

### Core Modules

| Module | Role |
|---|---|
| `toutch()` | Touch input processing |
| `mousepad()` | Mouse tracking and movement |
| `keys()` | Virtual keyboard HID mapping |
| `buttons()` | UI button creation |
| `initscreen()` | UI initialization |
| `state()` | System monitoring |
| `debug()` | Debug display |
| `UnitTab5Keyboard` | Physical keyboard integration |
| Custom USB HID layer | Keyboard and mouse USB reports |

The final implementation also includes dedicated handling for physical keyboard events and combined keyboard reports.

---

## ⚙️ Performance & Optimization

- ⚡ Minimal screen redraw
- 🔁 Fast input processing loop
- 🧠 State-based input logic
- ⌨️ Combined physical and virtual keyboard handling
- 🖱️ Optimized mouse movement and scrolling
- 🔌 USB HID report synchronization
- 🔋 Power-aware behavior
- 📉 Reduced unnecessary HID traffic when idle

The final firmware has demonstrated stable operation during regular use, with no crashes observed in testing.

---

## 📦 Included

- Original `.ino` source code (early virtual-keyboard architecture)
- Pre-final firmware build
- Final USB HID firmware binary
- Custom HID implementation in the final compiled firmware
- Project documentation
- Separate non-commercial license

**Note:** The publicly available `.ino` represents an earlier project architecture. The final USB HID firmware is distributed as a compiled binary.

---

## 🔧 Dependencies

For the Arduino-based source implementation:

- Arduino IDE
- ESP32 Arduino core
- M5Stack libraries:
  - `M5Unified`
  - `M5GFX`
- USB HID:
  - `USB.h`
  - Custom USB HID implementation
- Tab5KB keyboard support library for the final architecture

Precompiled firmware does not require users to install the Arduino development environment.

---

## 📦 Hardware

**Required**
- M5Stack Tab5 (ESP32-P4)
- Integrated touchscreen
- USB cable connected through the Tab5 USB-A port to the host

**Physical Keyboard**
- M5Stack Tab5KB

**Additional Hardware**
- Battery for portable operation and monitoring

The firmware monitors USB HID connection states and provides visual feedback through the Tab5KB RGB LEDs.

---

## 🛣️ Development Status

### ✅ Completed

- USB HID keyboard
- USB HID mouse
- On-screen AZERTY keyboard
- Physical Tab5KB integration
- Unified physical and virtual keyboard input
- Normal mouse mode
- Mixed mouse mode (Joystick + Mouse)
- Mouse inertia
- Integrated scrollbar
- Mouse speed adjustment
- Battery monitoring
- USB connection status monitoring
- Final USB firmware stabilization

### ❌ Not Planned

- BLE HID keyboard/mouse
- Unified USB + BLE system

Bluetooth HID development is not planned for this release. The project is intentionally focused on a complete and stable USB HID implementation.

**Project status: Final USB Release — Development Complete**

---

## ⚠️ Notes

- This project uses **USB HID only**.
- The final firmware supports both physical and virtual keyboard input.
- The mouse provides two distinct interaction modes.
- A USB connection to the host is required for HID input.
- The published original `.ino` does not represent the full final firmware architecture.
- No future BLE HID release is promised.

---

## 💡 Philosophy

This project explores:

- Embedded UI design
- Physical and virtual input integration
- USB HID abstraction
- Alternative mouse interaction modes
- Performance versus usability trade-offs
- Low-level input system control
- Embedded power management
- Stable standalone HID operation

The objective is not simply to emulate a keyboard or mouse, but to provide a complete portable HID control interface using the M5Stack Tab5.

---

## 👀 Preview

- On-screen AZERTY keyboard
- Physical Tab5KB keyboard
- Normal touchpad mouse
- Mixed joystick/mouse mode
- Mouse inertia
- Integrated scrollbar
- Battery and system monitoring
- USB HID connection feedback
- Debug information

---

**Tab5 HID — USB Full Final Release**

A standalone touchscreen and physical keyboard HID system for the M5Stack Tab5.
