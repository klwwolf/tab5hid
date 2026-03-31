#pragma once

#include "soc/soc_caps.h"
#if SOC_USB_OTG_SUPPORTED
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "Print.h"
#if CONFIG_TINYUSB_HID_ENABLED
#include "esp_event.h"
#include "class/hid/hid.h"
#include "class/hid/hid_device.h"

#define MOUSE_LEFT     0x01
#define MOUSE_RIGHT    0x02
#define MOUSE_MIDDLE   0x04
#define MOUSE_BACKWARD 0x08
#define MOUSE_FORWARD  0x10
#define MOUSE_ALL      0x1F

ESP_EVENT_DECLARE_BASE(ARDUINO_USB_HID_EVENTS);
ESP_EVENT_DECLARE_BASE(ARDUINO_USB_HID_KEYBOARD_EVENTS);

/*
  This file is not part of the public API. It is meant to be included only in Keyboard.cpp and the keyboard layout files.
  Layout files map ASCII character codes to keyboard scan codes (technically, to USB HID Usage codes), possibly altered by the SHIFT or ALT_GR modifiers.
  Non-ASCII characters (anything outside the 7-bit range NUL..DEL) are not supported.

  == Creating your own layout ==

  In order to create your own layout file, copy an existing layout that is similar to yours, then modify it to use the correct keys.
  The layout is an array in ASCII order. 
  Each entry contains a scan code, possibly modified by "|SHIFT" or "|ALT_GR", as in this excerpt from the Italian layout:

      0x35,          // bslash
      0x30|ALT_GR,   // ]
      0x2e|SHIFT,    // ^

  Do not change the control characters (those before scan code 0x2c, corresponding to space).
  Do not attempt to grow the table past DEL. 
  Do not use both SHIFT and ALT_GR on the same character: this is not supported. 
  Unsupported characters should have 0x00 as scan code.

  For a keyboard with an ISO physical layout, use the scan codes below:

      +---+---+---+---+---+---+---+---+---+---+---+---+---+-------+
      |35 |1e |1f |20 |21 |22 |23 |24 |25 |26 |27 |2d |2e |BackSp |
      +---+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-----+
      | Tab |14 |1a |08 |15 |17 |1c |18 |0c |12 |13 |2f |30 | Ret |
      +-----++--++--++--++--++--++--++--++--++--++--++--++--++    |
      |CapsL |04 |16 |07 |09 |0a |0b |0d |0e |0f |33 |34 |31 |    |
      +----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+---+----+
      |Shi.|32 |1d |1b |06 |19 |05 |11 |10 |36 |37 |38 |  Shift   |
      +----+---++--+-+-+---+---+---+---+---+--++---+---++----+----+
      |Ctrl|Win |Alt |                        |AlGr|Win |Menu|Ctrl|
      +----+----+----+------------------------+----+----+----+----+

  The ANSI layout is identical except that key 0x31 is above (rather than next to) Return, and there is not key 0x32.

  Give a unique name to the layout array, then declare it in Keyboard.h with a line of the form:

    extern const uint8_t KeyboardLayout_xx_YY[];

  == Encoding details ==

  All scan codes are less than 0x80, which makes bit 7 available to signal that a modifier (Shift or AltGr) is needed to generate the character. 
  With only one exception, keys that are used with modifiers have scan codes that are less than 0x40.
  This makes bit 6 available to signal whether the modifier is Shift or AltGr. 
  The exception is 0x64, the key next next to Left Shift on the ISO layout (and absent from the ANSI layout).
  We handle it by replacing its value by 0x32 in the layout arrays.
*/
#define SHIFT           0x80
#define ALT_GR          0x40
#define ISO_KEY         0x64
#define ISO_REPLACEMENT 0x32

enum {
  HID_REPORT_ID_NONE,
  HID_REPORT_ID_KEYBOARD,
  HID_REPORT_ID_MOUSE,
  HID_REPORT_ID_GAMEPAD,
  HID_REPORT_ID_CONSUMER_CONTROL,
  HID_REPORT_ID_SYSTEM_CONTROL,
  HID_REPORT_ID_VENDOR
};
typedef enum {
  ARDUINO_USB_HID_ANY_EVENT = ESP_EVENT_ANY_ID,
  ARDUINO_USB_HID_SET_PROTOCOL_EVENT = 0,
  ARDUINO_USB_HID_SET_IDLE_EVENT,
  ARDUINO_USB_HID_MAX_EVENT,
} arduino_usb_hid_event_t;
typedef enum {
  ARDUINO_USB_HID_KEYBOARD_ANY_EVENT = ESP_EVENT_ANY_ID,
  ARDUINO_USB_HID_KEYBOARD_LED_EVENT = 0,
  ARDUINO_USB_HID_KEYBOARD_MAX_EVENT,
} arduino_usb_hid_keyboard_event_t;
typedef struct {
  uint8_t instance;
  union {
    struct {uint8_t protocol;} set_protocol;
    struct {uint8_t idle_rate;} set_idle;
  };
} arduino_usb_hid_event_data_t;
typedef union {
  struct {
    uint8_t numlock    : 1;
    uint8_t capslock   : 1;
    uint8_t scrolllock : 1;
    uint8_t compose    : 1;
    uint8_t kana       : 1;
    uint8_t reserved   : 3;
  };
  uint8_t leds;
} arduino_usb_hid_keyboard_event_data_t;
enum MousePositioning_t {HID_MOUSE_RELATIVE,HID_MOUSE_ABSOLUTE};
struct HIDMouseType_t {
  MousePositioning_t positioning;
  const uint8_t *report_descriptor;
  size_t descriptor_size;
  size_t report_size;
};
extern HIDMouseType_t HIDMouseRel;
extern HIDMouseType_t HIDMouseAbs;
const uint8_t CUSTOMKeyboardLayout_en_US[128] PROGMEM = {
  0x00/*NUL*/,0x00/*SOH*/,0x00/*STX*/,0x00/*ETX*/,0x00/*EOT*/,0x00/*ENQ*/,0x00/*ACK*/,0x00/*BEL*/,
  0x2a/*BS  Backspace*/,0x2b/*TAB Tab*/,0x28/*LF  Enter*/,0x00/*VT*/,0x00/*FF*/,0x00/*CR*/,
  0x00,// SO
  0x00,// SI
  0x00,// DEL
  0x00,// DC1
  0x00,// DC2
  0x00,// DC3
  0x00,// DC4
  0x00,// NAK
  0x00,// SYN
  0x00,// ETB
  0x00,// CAN
  0x00,// EM
  0x00,// SUB
  0x00,// ESC
  0x00,// FS
  0x00,// GS
  0x00,// RS
  0x00,// US
  0x2c,// ' '

  0x1e | SHIFT,// !
  0x34 | SHIFT,// "
  0x20 | SHIFT,// #
  0x21 | SHIFT,// $
  0x22 | SHIFT,  // %
  0x24 | SHIFT,  // &
  0x34, // '
  0x26 | SHIFT,  // (
  0x27 | SHIFT,  // )
  0x25 | SHIFT,  // *
  0x2e | SHIFT,  // +
  0x36,          // ,
  0x2d,          // -
  0x37,          // .
  0x38,          // /
  0x27,          // 0
  0x1e,          // 1
  0x1f,          // 2
  0x20,          // 3
  0x21,          // 4
  0x22,          // 5
  0x23,          // 6
  0x24,          // 7
  0x25,          // 8
  0x26,          // 9
  0x33 | SHIFT,  // :
  0x33,          // ;
  0x36 | SHIFT,  // <
  0x2e,          // =
  0x37 | SHIFT,  // >
  0x38 | SHIFT,  // ?
  0x1f | SHIFT,  // @
  0x04 | SHIFT,  // A
  0x05 | SHIFT,  // B
  0x06 | SHIFT,  // C
  0x07 | SHIFT,  // D
  0x08 | SHIFT,  // E
  0x09 | SHIFT,  // F
  0x0a | SHIFT,  // G
  0x0b | SHIFT,  // H
  0x0c | SHIFT,  // I
  0x0d | SHIFT,  // J
  0x0e | SHIFT,  // K
  0x0f | SHIFT,  // L
  0x10 | SHIFT,  // M
  0x11 | SHIFT,  // N
  0x12 | SHIFT,  // O
  0x13 | SHIFT,  // P
  0x14 | SHIFT,  // Q
  0x15 | SHIFT,  // R
  0x16 | SHIFT,  // S
  0x17 | SHIFT,  // T
  0x18 | SHIFT,  // U
  0x19 | SHIFT,  // V
  0x1a | SHIFT,  // W
  0x1b | SHIFT,  // X
  0x1c | SHIFT,  // Y
  0x1d | SHIFT,  // Z
  0x2f,          // [
  0x31,          // bslash
  0x30,          // ]
  0x23 | SHIFT,  // ^
  0x2d | SHIFT,  // _
  0x35,          // `
  0x04,          // a
  0x05,          // b
  0x06,          // c
  0x07,          // d
  0x08,          // e
  0x09,          // f
  0x0a,          // g
  0x0b,          // h
  0x0c,          // i
  0x0d,          // j
  0x0e,          // k
  0x0f,          // l
  0x10,          // m
  0x11,          // n
  0x12,          // o
  0x13,          // p
  0x14,          // q
  0x15,          // r
  0x16,          // s
  0x17,          // t
  0x18,          // u
  0x19,          // v
  0x1a,          // w
  0x1b,          // x
  0x1c,          // y
  0x1d,          // z
  0x2f | SHIFT,  // {
  0x31 | SHIFT,  // |
  0x30 | SHIFT,  // }
  0x35 | SHIFT,  // ~
  0x00           // DEL
};


typedef struct {
  uint8_t modifiers;uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

class CUSTOMUSBHIDDevice {
  public:
    virtual uint16_t _onGetDescriptor(uint8_t *buffer) {return 0;}
    virtual uint16_t _onGetFeature(uint8_t report_id, uint8_t *buffer, uint16_t len) {return 0;}
    virtual void _onSetFeature(uint8_t report_id, const uint8_t *buffer, uint16_t len) {}
    virtual void _onOutput(uint8_t report_id, const uint8_t *buffer, uint16_t len) {}
};
class CUSTOMUSBHID {
  public:
    CUSTOMUSBHID(hid_interface_protocol_enum_t itf_protocol = HID_ITF_PROTOCOL_NONE);
    void begin(void);
    void end(void);
    bool ready(void);
    bool SendReport(uint8_t report_id, const void *data, size_t len, uint32_t timeout_ms = 100);
    void onEvent(esp_event_handler_t callback);
    void onEvent(arduino_usb_hid_event_t event, esp_event_handler_t callback);
    static bool addDevice(CUSTOMUSBHIDDevice *device, uint16_t descriptor_len);
};
class CUSTOMUSBHIDMouseBase : public CUSTOMUSBHIDDevice {
  public:
    CUSTOMUSBHIDMouseBase(HIDMouseType_t *type);
    void begin(void);
    void end(void);
    void press(uint8_t b = MOUSE_LEFT);      // press LEFT by default
    void release(uint8_t b = MOUSE_LEFT);    // release LEFT by default
    bool isPressed(uint8_t b = MOUSE_LEFT);  // check LEFT by default
    template<typename T> bool sendReport(T report) {return hid.SendReport(HID_REPORT_ID_MOUSE, &report, _type->report_size);};
    uint16_t _onGetDescriptor(uint8_t *buffer);
    virtual void click(uint8_t b) = 0;
    virtual void buttons(uint8_t b) = 0;
  protected:
    CUSTOMUSBHID hid;
    uint8_t _buttons;
    HIDMouseType_t *_type;
};
class CUSTOMUSBHIDRelativeMouse : public CUSTOMUSBHIDMouseBase {
  public:
    CUSTOMUSBHIDRelativeMouse(void) : CUSTOMUSBHIDMouseBase(&HIDMouseRel) {}
    void move(int8_t x, int8_t y, int8_t wheel = 0, int8_t pan = 0);
    void click(uint8_t b = MOUSE_LEFT) override;
    void buttons(uint8_t b) override;
};
class CUSTOMUSBHIDAbsoluteMouse : public CUSTOMUSBHIDMouseBase {
  public:
    CUSTOMUSBHIDAbsoluteMouse(void) : CUSTOMUSBHIDMouseBase(&HIDMouseAbs) {}
    void move(int16_t x, int16_t y, int8_t wheel = 0, int8_t pan = 0);
    void click(uint8_t b = MOUSE_LEFT) override;
    void buttons(uint8_t b) override;
  private:
    int16_t _lastx = 0;
    int16_t _lasty = 0;
};
class CUSTOMUSBHIDKeyboard : public CUSTOMUSBHIDDevice, public Print {
  private:
    CUSTOMUSBHID hid;
    KeyReport _keyReport;
    const uint8_t *_asciimap;
    bool shiftKeyReports;
  public:
    CUSTOMUSBHIDKeyboard(void);
    void begin(const uint8_t *layout = CUSTOMKeyboardLayout_en_US);
    void end(void);
    size_t write(uint8_t k);
    size_t write(const uint8_t *buffer, size_t size);
    size_t press(uint8_t k);
    size_t release(uint8_t k);
    void releaseAll(void);
    void sendReport(KeyReport *keys);
    void setShiftKeyReports(bool set);
    size_t pressRaw(uint8_t k);
    size_t releaseRaw(uint8_t k);
    void onEvent(esp_event_handler_t callback);
    void onEvent(arduino_usb_hid_keyboard_event_t event, esp_event_handler_t callback);
    uint16_t _onGetDescriptor(uint8_t *buffer);
    void _onOutput(uint8_t report_id, const uint8_t *buffer, uint16_t len);
};

typedef CUSTOMUSBHIDRelativeMouse CUSTOMUSBHIDMouse;

#endif /* CONFIG_TINYUSB_HID_ENABLED */
#endif /* SOC_USB_OTG_SUPPORTED */