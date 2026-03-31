#ifndef TAB5_BLE_CORE_H
#define TAB5_BLE_CORE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEHIDDevice.h>
#include <BLESecurity.h>
#include <HIDTypes.h>
#include "keyreg.h"

class Tab5BLECore {
  public:
    //callback
    friend class ServerCallbackscore;

    //enum
    enum Modifier : uint8_t {
      MOD_NONE=keyreg::NONE,
      MOD_LCTRL=keyreg::LCTRL,MOD_LSHIFT=keyreg::LSHIFT,MOD_LALT=keyreg::LALT,MOD_LGUI=keyreg::LGUI,
      MOD_RCTRL=keyreg::RCTRL,MOD_RSHIFT=keyreg::RSHIFT,MOD_RALT=keyreg::RALT,MOD_RGUI=keyreg::RGUI
    };
    enum Key : uint8_t {
      C_KEY_ENTER=0x28,C_KEY_TAB=0xB3,C_KEY_SPACE=0x2C,C_KEY_ESC=0x29,C_KEY_BACKSPACE=0xB2,C_KEY_DELETE=0xD4,
      C_KEY_RIGHT_ARROW=0xD7,C_KEY_LEFT_ARROW=0xD8,C_KEY_DOWN_ARROW=0xD9,C_KEY_UP_ARROW=0xDA
    };
    enum MouseButton : uint8_t {M_LEFT=0x01,M_RIGHT =0x02,M_MIDDLE =0x04,M_BACK=0x08,M_FORWARD=0x10};

    //reportstruct
    struct KeyReport {uint8_t modifiers;uint8_t reserved;uint8_t keys[6];};
    struct MouseReport {uint8_t buttons;int8_t x;int8_t y;int8_t wheel;};
    struct UnifiedReport {uint8_t modifiers;uint8_t reserved;uint8_t keys[6];uint8_t buttons;int8_t x;int8_t y;int8_t wheel;};

    //mode
    bool _gene;
    void setgene(bool mode);
    bool getgene() const;
    bool _unified;
    void setunified(bool mode);
    bool getunified() const;

    //logic
    bool kbmod(uint8_t key);
    bool ifcondms() const;
    bool ifcondkb() const;
    bool checkms() const;
    bool checkkb() const;

    //begin
    uint16_t _vid;
    uint16_t _pid;
    uint16_t _ver;
    Tab5BLECore(uint16_t vid=0x1234,uint16_t pid=0x0001,uint16_t ver=0x0100);

    //bluetooth
    void begin(const char* deviceName="Tab5BLECore",const char* manufacturer="M5Stack",bool enableBond=true);
    bool isBegin() const;
    void disableBluetooth();
    void disconnectdevice();
    bool isConnected() const;
    void setBatteryLevel(uint8_t percent);
    void setBattery(uint8_t percent);
    void handleConnect();
    void handleDisconnect();
    BLEHIDDevice* _hidDevice;
    BLECharacteristic* _inputKeyboard;
    BLECharacteristic* _inputMouse;
    BLECharacteristic* _inputGeneric;
    BLEAdvertising* _advertising;
    BLEServer* _server;
    static Tab5BLECore* _instance;
    uint16_t _connId;
    bool _connected;
    bool _begun;

    //key
    bool releaseAll();
    bool addKey(uint8_t keycode);
    bool removeKey(uint8_t keycode);
    bool containsKey(uint8_t keycode);

    //report
    bool sendReportkb();
    bool sendReportms();
    bool sendReportuni();
    KeyReport _kreport;
    MouseReport _mreport;
    UnifiedReport _ureport;

    //keyboard
    bool write(uint8_t keycode);
    bool tap(uint8_t keycode,uint16_t holdMs=8);
    bool press(uint8_t keycode);
    bool pressKey(uint8_t keycode);
    bool pressModifier(uint8_t mod);
    bool release(uint8_t keycode);
    bool releaseKey(uint8_t keycode);
    bool releaseModifier(uint8_t mod);
    static bool asciiToHid(char c,uint8_t& key,uint8_t& mod);
    bool writeAscii(char c,uint16_t holdMs=8);

    //mouse
    void move(int8_t x,int8_t y,int8_t wheel=0);
    void pressButton(uint8_t button);
    void releaseButton(uint8_t button);
    bool isButtonPressed(uint8_t button);
    void click(uint8_t button,uint16_t holdMs=10);
};

#endif