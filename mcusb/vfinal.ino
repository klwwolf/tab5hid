#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedKEYBOARD.h>
#include <M5HAL.hpp>
#include <M5Utility.h>
#include <algorithm>
#include <cctype>
#include <string>
#include "USB.h"

bool lockR=false,lockL=false,lockSS=false,lockMMD=false,lockCharge=false,majlock=false,chargemode=true;
bool lockKey[60]={false},onToutch[9]={false},keyTouched[49]={false};
unsigned long plX=-1,plY=-1,lss=-1,X=900,DX=400,DY=30,sensy=1,kbtp=1,autosleep=0,timer=0;
int padX=0,padY=0,scroll=0,lasttoutch=0,kbfstt=0,ld1info=0,remain=30;
const int sizea=20;
String txt[sizea];
const char* AZERTYUP="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* AZERTYDOWN="abcdefghijklmnopqrstuvwxyz";
const char* NUMBER="1234567890";
static const char num1[10]={'1','2','3','4','5','6','7','8','9','0'};

class rg2 {
  public:
    //change from keyreg
    enum Modifier:uint8_t{NONE=0x00,LCTRL=0x80,LSHIFT=0x81,LALT=0x82,LGUI=0x83,RCTRL=0x84,RSHIFT=0x85,RALT=0x86,RGUI=0x87};// RALT = AltGr
    //unchange from keyreg
    enum Keynumber:uint8_t{NUM_1=0x1E,NUM_2=0x1F,NUM_3=0x20,NUM_4=0x21,NUM_5=0x22,NUM_6=0x23,NUM_7=0x24,NUM_8=0x25,NUM_9=0x26,NUM_0=0x27};
    enum Keymin:uint8_t{a=0x04,b=0x05,c=0x06,d=0x07,e=0x08,f=0x09,g=0x0A,h=0x0B,i=0x0C,j=0x0D,k=0x0E,l=0x0F,m=0x10,n=0x11,o=0x12,p=0x13,q=0x14,r=0x15,s=0x16,t=0x17,u=0x18,v=0x19,w=0x1A,x=0x1B,y=0x1C,z=0x1D};
    enum Keymaj:uint8_t{A=0x04,B=0x05,C=0x06,D=0x07,E=0x08,F=0x09,G=0x0A,H=0x0B,I=0x0C,J=0x0D,K=0x0E,L=0x0F,M=0x10,N=0x11,O=0x12,P=0x13,Q=0x14,R=0x15,S=0x16,T=0x17,U=0x18,V=0x19,W=0x1A,X=0x1B,Y=0x1C,Z=0x1D};
    //change from keyreg
    enum Keycontrol:uint8_t{ENTER=0xB0,ESC=0xB1,BACKSPACE=0xB2,TAB=0xB3,SPACE=0x20};
    //half change from keyreg
    enum Keysymbols:uint8_t{
      MINUS=0x2D,EQUAL=0x2E,LBRACKET=0x2F,RBRACKET=0x30,BACKSLASH=0x31,NON_US=0x32,SEMICOLON=0x33,QUOTE=0x34,GRAVE=0x35,COMMA=0x36,DOT=0x37,SLASH=0x38, //unchanged
      CAPSLOCK=0xC1 //changed
    };
    //change from keyreg
    enum KeyFA:uint8_t{F1=0xC2,F2=0xC3,F3=0xC4,F4=0xC5,F5=0xC6,F6=0xC7,F7=0xC8,F8=0xC9,F9=0xCA,F10=0xCB,F11=0xCC,F12=0xCD};
    enum KeyFB:uint8_t{F13=0xF0,F14=0xF1,F15=0xF2,F16=0xF3,F17=0xF4,F18=0xF5,F19=0xF6,F20=0xF7,F21=0xF8,F22=0xF9,F23=0xFA,F24=0xFB};
    enum KeyArrow:uint8_t{RIGHT=0xD7,LEFT=0xD8,DOWN=0xD9,UP=0xDA};
    enum KeyNav:uint8_t{INSERT=0xD1,HOME=0xD2,PAGE_UP=0xD3,DELETE=0xD4,END=0xD5,PAGE_DOWN=0xD6};
    enum KeyNumpadcmd:uint8_t{KP_NUMLOCK=0xDB,KP_SLASH=0xDC,KP_ASTERISK=0xDD,KP_MINUS=0xDE,KP_PLUS=0xDF,KP_ENTER=0xE0,KP_DOT=0xEB};
    enum KeyNumpadnum:uint8_t{KP_1=0xE1,KP_2=0xE2,KP_3=0xE3,KP_4=0xE4,KP_5=0xE5,KP_6=0xE6,KP_7=0xE7,KP_8=0xE8,KP_9=0xE9,KP_0=0xEA};
    enum KeySystem:uint8_t{PRINT_SCREEN=0xCE,SCROLL_LOCK=0xCF,PAUSE=0xD0,MENU=0xED};
    enum Led:uint8_t{NUMLOCK=0x01,CAPSLOCKLED=0x02,SCROLLLOCK=0x04,COMPOSE=0x08,KANA=0x10};
    //keyevent
    struct KeyEvent1{uint8_t Keynumber;uint8_t modifier;KeyEvent1(uint8_t k=0,uint8_t m=0):Keynumber(k),modifier(m){}};
    struct KeyEvent2{uint8_t Keymin;uint8_t modifier;KeyEvent2(uint8_t k=0,uint8_t m=0):Keymin(k),modifier(m){}};
    struct KeyEvent3{uint8_t Keymaj;uint8_t modifier;KeyEvent3(uint8_t k=0,uint8_t m=0):Keymaj(k),modifier(m){}};
    struct KeyEvent4{uint8_t Keycontrol;uint8_t modifier;KeyEvent4(uint8_t k=0,uint8_t m=0):Keycontrol(k),modifier(m){}};
    struct KeyEvent5{uint8_t Keysymbols;uint8_t modifier;KeyEvent5(uint8_t k=0,uint8_t m=0):Keysymbols(k),modifier(m){}};
};

namespace custom{

  #include "soc/soc_caps.h"
  #if SOC_USB_OTG_SUPPORTED
  #include <stdint.h>
  #include <stdbool.h>
  #include "sdkconfig.h"
  #include "Print.h"
  #if CONFIG_TINYUSB_HID_ENABLED
  #include "esp_event.h"
  #include "class/hid/hid.h"
  #include "USB.h"
  #include "class/hid/hid_device.h"
  #include "esp32-hal-tinyusb.h"
  #include "esp_hid_common.h"

  uint8_t MOUSE_LEFT=0x01;
  uint8_t MOUSE_RIGHT=0x02;
  uint8_t MOUSE_MIDDLE=0x04;
  uint8_t MOUSE_BACKWARD =0x08;
  uint8_t MOUSE_FORWARD=0x10;
  uint8_t MOUSE_ALL=0x1F;

  uint8_t SHIFT=0x80;
  uint8_t ALT_GR=0x40;
  uint8_t ISO_KEY=0x64;
  uint8_t ISO_REPLACEMENT= 0x32;

  #define USB_HID_DEVICES_MAX 10

  ESP_EVENT_DECLARE_BASE(ARDUINO_USB_HID_EVENTS);
  ESP_EVENT_DECLARE_BASE(ARDUINO_USB_HID_KEYBOARDV_EVENTS);
  ESP_EVENT_DECLARE_BASE(ARDUINO_USB_HID_KEYBOARDP_EVENTS);

  enum {HID_REPORT_ID_NONE_s,HID_REPORT_ID_KEYBOARD_s,HID_REPORT_ID_MOUSE_s,HID_REPORT_ID_GAMEPAD_s,HID_REPORT_ID_CONSUMER_CONTROL_s,HID_REPORT_ID_SYSTEM_CONTROL_s,HID_REPORT_ID_VENDOR_s};

  enum MousePositioning_t {HID_MOUSE_RELATIVE,HID_MOUSE_ABSOLUTE};
  struct HIDMouseType_t {MousePositioning_t positioning;const uint8_t *report_descriptor;size_t descriptor_size;size_t report_size;};
  extern HIDMouseType_t HIDMouseRel;
  extern HIDMouseType_t HIDMouseAbs;
  static const uint8_t abs_mouse_report_descriptor[] = {TUD_HID_REPORT_DESC_ABSMOUSE(HID_REPORT_ID(HID_REPORT_ID_MOUSE_s))};
  HIDMouseType_t HIDMouseAbs = {HID_MOUSE_ABSOLUTE, abs_mouse_report_descriptor, sizeof(abs_mouse_report_descriptor), sizeof(hid_abs_mouse_report_t)};
  static const uint8_t rel_mouse_report_descriptor[] = {TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_REPORT_ID_MOUSE_s))};
  HIDMouseType_t HIDMouseRel = {HID_MOUSE_RELATIVE, rel_mouse_report_descriptor, sizeof(rel_mouse_report_descriptor), sizeof(hid_mouse_report_t)};

  static const uint8_t report_descriptor[] = {TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_REPORT_ID_KEYBOARD_s))};

  typedef enum {ARDUINO_USB_HID_KEYBOARD_ANY_EVENT_s = ESP_EVENT_ANY_ID,ARDUINO_USB_HID_KEYBOARD_LED_EVENT_s = 0,ARDUINO_USB_HID_KEYBOARD_MAX_EVENT_s,} arduino_usb_hid_keyboard_event_t_s;
  typedef union {struct {uint8_t numlock:1;uint8_t capslock:1;uint8_t scrolllock:1;uint8_t compose:1;uint8_t kana:1;uint8_t reserved:3;};uint8_t leds;} arduino_usb_hid_keyboard_event_data_t_s;
  typedef struct {uint8_t modifiers;uint8_t reserved;uint8_t keys[6];} KeyReports;

  typedef struct {uint8_t instance;union {struct {uint8_t protocol;} set_protocol;struct {uint8_t idle_rate;} set_idle;};} arduino_usb_hid_event_data_t_s;

  class USBHIDDevice {
    public:
      virtual uint16_t _onGetDescriptor(uint8_t *buffer) {return 0;}
      virtual uint16_t _onGetFeature(uint8_t report_id,uint8_t *buffer,uint16_t len) {return 0;}
      virtual void _onSetFeature(uint8_t report_id,const uint8_t *buffer,uint16_t len) {}
      virtual void _onOutput(uint8_t report_id,const uint8_t *buffer,uint16_t len) {}
  };

  static esp_event_loop_handle_t arduino_usb_event_loop_handle = NULL;

  esp_err_t arduino_usb_event_post(esp_event_base_t event_base, int32_t event_id, void *event_data, size_t event_data_size, TickType_t ticks_to_wait) {
    if (arduino_usb_event_loop_handle == NULL) {return ESP_FAIL;}
    return esp_event_post_to(arduino_usb_event_loop_handle, event_base, event_id, event_data, event_data_size, ticks_to_wait);
  }
  esp_err_t arduino_usb_event_handler_register_with(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler, void *event_handler_arg) {
    if (arduino_usb_event_loop_handle == NULL) {return ESP_FAIL;}
    return esp_event_handler_register_with(arduino_usb_event_loop_handle, event_base, event_id, event_handler, event_handler_arg);
  }
  typedef struct {USBHIDDevice *device;uint8_t reports_num;uint8_t *report_ids;} tinyusb_hid_device_t;
  static tinyusb_hid_device_t tinyusb_hid_devices[USB_HID_DEVICES_MAX];
  static uint8_t tinyusb_hid_devices_num = 0;
  static bool tinyusb_hid_devices_is_initialized = false;
  static SemaphoreHandle_t tinyusb_hid_device_input_sem = NULL;
  static SemaphoreHandle_t tinyusb_hid_device_input_mutex = NULL;
  static bool tinyusb_hid_is_initialized = false;
  static hid_interface_protocol_enum_t tinyusb_interface_protocol = HID_ITF_PROTOCOL_NONE;
  static uint8_t tinyusb_loaded_hid_devices_num = 0;
  static uint16_t tinyusb_hid_device_descriptor_len = 0;
  static uint8_t *tinyusb_hid_device_descriptor = NULL;
  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
    static const char *tinyusb_hid_device_report_types[4] = {"INVALID", "INPUT", "OUTPUT", "FEATURE"};
  #endif
  template<class F> struct ArgType;
  template<class R, class T1, class T2, class T3> struct ArgType<R (*)(T1, T2, T3)> {typedef T1 type1;typedef T2 type2;typedef T3 type3;};
  typedef ArgType<decltype(&tud_hid_report_complete_cb)>::type3 tud_hid_report_complete_cb_len_t;

  typedef enum {ARDUINO_USB_HID_ANY_EVENT_s = ESP_EVENT_ANY_ID,ARDUINO_USB_HID_SET_PROTOCOL_EVENT_s = 0,ARDUINO_USB_HID_SET_IDLE_EVENT_s,ARDUINO_USB_HID_MAX_EVENT_s,} arduino_usb_hid_event_t_s;

  USBHIDDevice *tinyusb_get_device_by_report_id(uint8_t report_id){
    for(uint8_t i=0;i<tinyusb_loaded_hid_devices_num;i++){
      tinyusb_hid_device_t *device=&tinyusb_hid_devices[i];
      if(device->device&&device->reports_num)for(uint8_t r=0;r<device->reports_num;r++)if(report_id==device->report_ids[r]){return device->device;}
    }
    return NULL;
  }
  
  static bool tinyusb_enable_hid_device(uint16_t descriptor_len, USBHIDDevice *device) {
    if (tinyusb_hid_is_initialized) {log_e("TinyUSB HID has already started! Device not enabled");return false;}
    if (tinyusb_loaded_hid_devices_num >= USB_HID_DEVICES_MAX) {log_e("Maximum devices already enabled! Device not enabled");return false;}
    tinyusb_hid_device_descriptor_len += descriptor_len;
    tinyusb_hid_devices[tinyusb_loaded_hid_devices_num++].device = device;
    log_d("Device[%u] len: %u", tinyusb_loaded_hid_devices_num - 1, descriptor_len);
    return true;
  }
  static uint16_t tinyusb_on_get_feature(uint8_t report_id,uint8_t *buffer,uint16_t reqlen){
    USBHIDDevice *device=tinyusb_get_device_by_report_id(report_id);if(device){return device->_onGetFeature(report_id,buffer,reqlen);}return 0;
  }
  static bool tinyusb_on_set_feature(uint8_t report_id,const uint8_t *buffer,uint16_t reqlen){
    USBHIDDevice *device=tinyusb_get_device_by_report_id(report_id);if(device){device->_onSetFeature(report_id,buffer,reqlen);return true;}return false;
  }
  static bool tinyusb_on_set_output(uint8_t report_id,const uint8_t *buffer,uint16_t reqlen){
    USBHIDDevice *device=tinyusb_get_device_by_report_id(report_id);if(device){device->_onOutput(report_id,buffer,reqlen);return true;}return false;
  }
  static uint16_t tinyusb_on_add_descriptor(uint8_t device_index,uint8_t *dst){
    uint16_t res=0;uint8_t report_id=0,reports_num=0;
    tinyusb_hid_device_t *device=&tinyusb_hid_devices[device_index];
    if(device->device){
      res=device->device->_onGetDescriptor(dst);if(res){
        esp_hid_report_map_t *hid_report_map=esp_hid_parse_report_map(dst,res);
        if(hid_report_map){
          if(device->report_ids){free(device->report_ids);}
          device->reports_num=hid_report_map->reports_len;
          device->report_ids=(uint8_t *)malloc(device->reports_num);
          memset(device->report_ids,0,device->reports_num);
          reports_num=device->reports_num;
          for(uint8_t i=0;i<device->reports_num;i++){if (hid_report_map->reports[i].protocol_mode==ESP_HID_PROTOCOL_MODE_REPORT) {
            report_id = hid_report_map->reports[i].report_id;
            for(uint8_t r=0;r<device->reports_num;r++){if(!report_id){break;}else if(report_id==device->report_ids[r]){reports_num--;break;}else if(!device->report_ids[r]){device->report_ids[r]=report_id;break;}}
          }else{reports_num--;}}
          device->reports_num=reports_num;
          esp_hid_free_report_map(hid_report_map);
        }
      }
    }
    return res;
  }
  static bool tinyusb_load_enabled_hid_devices() {
    if(tinyusb_hid_device_descriptor!=NULL){return true;}
    tinyusb_hid_device_descriptor=(uint8_t *)malloc(tinyusb_hid_device_descriptor_len);
    if (tinyusb_hid_device_descriptor==NULL){log_e("HID Descriptor Malloc Failed");return false;}
    uint8_t *dst=tinyusb_hid_device_descriptor;
    for (uint8_t i=0;i<tinyusb_loaded_hid_devices_num;i++){uint16_t len=tinyusb_on_add_descriptor(i,dst);if(!len){break;}else{dst+=len;}}
    esp_hid_report_map_t *hid_report_map=esp_hid_parse_report_map(tinyusb_hid_device_descriptor,tinyusb_hid_device_descriptor_len);
    if(hid_report_map){
      log_d("Loaded HID Descriptor with the following reports:");
      for (uint8_t i = 0; i < hid_report_map->reports_len; i++) {if (hid_report_map->reports[i].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {log_d(
        "  ID: %3u, Type: %7s, Size: %2u, Usage: %8s", 
        hid_report_map->reports[i].report_id, esp_hid_report_type_str(hid_report_map->reports[i].report_type),hid_report_map->reports[i].value_len, esp_hid_usage_str(hid_report_map->reports[i].usage)
      );}}
      esp_hid_free_report_map(hid_report_map);
    } else {log_e("Failed to parse the hid report descriptor!");return false;}

    return true;
  }
  extern "C" uint16_t tusb_hid_load_descriptor(uint8_t *dst, uint8_t *itf) {
    if (tinyusb_hid_is_initialized) {
      return 0;
    }
    tinyusb_hid_is_initialized = true;

    uint8_t str_index = tinyusb_add_string_descriptor("TinyUSB HID");
    // For keyboard boot protocol, we've already called tinyusb_enable_interface2(reserve_endpoints=true)
    uint8_t ep_in = tinyusb_interface_protocol == HID_ITF_PROTOCOL_KEYBOARD ? 1 : tinyusb_get_free_in_endpoint();
    TU_VERIFY(ep_in != 0);
    uint8_t ep_out = tinyusb_interface_protocol == HID_ITF_PROTOCOL_KEYBOARD ? 1 : tinyusb_get_free_out_endpoint();
    TU_VERIFY(ep_out != 0);
    uint8_t descriptor[TUD_HID_INOUT_DESC_LEN] = {
      // HID Input & Output descriptor
      // Interface number, string index, protocol, report descriptor len, EP OUT & IN address, size & polling interval
      TUD_HID_INOUT_DESCRIPTOR(
        *itf, str_index, tinyusb_interface_protocol, tinyusb_hid_device_descriptor_len, ep_out, (uint8_t)(0x80 | ep_in), CFG_TUD_ENDOINT_SIZE, 1
      )
    };
    *itf += 1;
    memcpy(dst, descriptor, TUD_HID_INOUT_DESC_LEN);
    return TUD_HID_INOUT_DESC_LEN;
  }
  uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    log_v("instance: %u", instance);
    if (!tinyusb_load_enabled_hid_devices()) {
      return NULL;
    }
    return tinyusb_hid_device_descriptor;
  }
  void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
    log_v("instance: %u, protocol:%u", instance, protocol);
    arduino_usb_hid_event_data_t_s p;
    p.instance = instance;
    p.set_protocol.protocol = protocol;
    arduino_usb_event_post(ARDUINO_USB_HID_EVENTS, ARDUINO_USB_HID_SET_PROTOCOL_EVENT_s, &p, sizeof(arduino_usb_hid_event_data_t_s), portMAX_DELAY);
  }
  bool tud_hid_set_idle_cb(uint8_t instance, uint8_t idle_rate) {
    log_v("instance: %u, idle_rate:%u", instance, idle_rate);
    arduino_usb_hid_event_data_t_s p;
    p.instance = instance;
    p.set_idle.idle_rate = idle_rate;
    arduino_usb_event_post(ARDUINO_USB_HID_EVENTS, ARDUINO_USB_HID_SET_IDLE_EVENT_s, &p, sizeof(arduino_usb_hid_event_data_t_s), portMAX_DELAY);
    return true;
  }
  uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    uint16_t res = tinyusb_on_get_feature(report_id, buffer, reqlen);
    if (!res) {
      log_d("instance: %u, report_id: %u, report_type: %s, reqlen: %u", instance, report_id, tinyusb_hid_device_report_types[report_type], reqlen);
    }
    return res;
  }
  void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    if (!report_id && (!report_type || report_type == HID_REPORT_TYPE_OUTPUT)) {
      if (!tinyusb_on_set_output(0, buffer, bufsize) && !tinyusb_on_set_output(buffer[0], buffer + 1, bufsize - 1)) {
        log_d(
          "instance: %u, report_id: %u, report_type: %s, bufsize: %u", instance, buffer[0], tinyusb_hid_device_report_types[HID_REPORT_TYPE_OUTPUT], bufsize - 1
        );
      }
    } else {
      if (!tinyusb_on_set_feature(report_id, buffer, bufsize)) {
        log_d("instance: %u, report_id: %u, report_type: %s, bufsize: %u", instance, report_id, tinyusb_hid_device_report_types[report_type], bufsize);
      }
    }
  }
  void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, tud_hid_report_complete_cb_len_t len) {if (tinyusb_hid_device_input_sem) {xSemaphoreGive(tinyusb_hid_device_input_sem);}}

  class USBHID {
    public:
      USBHID(hid_interface_protocol_enum_t itf_protocol =HID_ITF_PROTOCOL_NONE) {
        if (!tinyusb_hid_devices_is_initialized) {
          tinyusb_hid_devices_is_initialized = true;
          for (uint8_t i = 0; i < USB_HID_DEVICES_MAX; i++) {memset(&tinyusb_hid_devices[i],0,sizeof(tinyusb_hid_device_t));}
          tinyusb_hid_devices_num = 0;
          tinyusb_interface_protocol = itf_protocol;
          tinyusb_enable_interface2(USB_INTERFACE_HID,TUD_HID_INOUT_DESC_LEN,tusb_hid_load_descriptor,itf_protocol == HID_ITF_PROTOCOL_KEYBOARD);
        }
      }
      void begin() {
          if (tinyusb_hid_device_input_sem == NULL) {tinyusb_hid_device_input_sem =xSemaphoreCreateBinary();}
          if (tinyusb_hid_device_input_mutex == NULL) {tinyusb_hid_device_input_mutex =xSemaphoreCreateMutex();}
      }
      void end() {
          if (tinyusb_hid_device_input_sem != NULL) {vSemaphoreDelete(tinyusb_hid_device_input_sem);tinyusb_hid_device_input_sem = NULL;}
          if (tinyusb_hid_device_input_mutex != NULL) {vSemaphoreDelete(tinyusb_hid_device_input_mutex);tinyusb_hid_device_input_mutex = NULL;}
      }
      bool SendReport(uint8_t id,const void *data,size_t len,uint32_t timeout_ms) {
          if (!tinyusb_hid_device_input_sem ||!tinyusb_hid_device_input_mutex) {log_e("TX Semaphore is NULL. You must call USBHID::begin() before you can send reports");return false;}
          if (xSemaphoreTake(tinyusb_hid_device_input_mutex,timeout_ms / portTICK_PERIOD_MS) != pdTRUE) {log_e("report %u mutex failed", id);return false;}
          uint8_t effective_id =((tinyusb_interface_protocol !=HID_ITF_PROTOCOL_NONE)&&(tud_hid_n_get_protocol(0) ==HID_PROTOCOL_BOOT))? 0: id;
          bool res = ready();
          if (!res) {log_e("not ready");}else {
              xSemaphoreTake(tinyusb_hid_device_input_sem,0);
              res = tud_hid_n_report(0,effective_id,data,len);
              if (!res) {log_e("report %u failed", id);}else {if (xSemaphoreTake(tinyusb_hid_device_input_sem,timeout_ms / portTICK_PERIOD_MS) != pdTRUE) {log_e("report %u wait failed",id);res = false;}}
          }
          xSemaphoreGive(tinyusb_hid_device_input_mutex);
          return res;
      }
      bool ready() {return tud_hid_n_ready(0);}
      static bool addDevice(USBHIDDevice *device,uint16_t descriptor_len) {
        if (device && tinyusb_loaded_hid_devices_num < USB_HID_DEVICES_MAX) {
          if (!tinyusb_enable_hid_device(descriptor_len, device)) {return false;}
          return true;
        }
        return false;
      }
      void onEvent(esp_event_handler_t callback) {onEvent(ARDUINO_USB_HID_ANY_EVENT_s,callback);}
      void onEvent(arduino_usb_hid_event_t_s event,esp_event_handler_t callback) {arduino_usb_event_handler_register_with(ARDUINO_USB_HID_EVENTS,event,callback,this);}
  };

  class USBHIDMouseBase : public USBHIDDevice {
    public:
      USBHIDMouseBase(HIDMouseType_t *type): hid(), _buttons(0), _type(type){
        static bool initialized = false;
        if (!initialized) {initialized = true;hid.addDevice(this,_type->descriptor_size);}
      }
      void begin(void) {hid.begin();}
      void end(void) {}
      void press(uint8_t b = MOUSE_LEFT) {this->buttons(_buttons | b);}
      void release(uint8_t b = MOUSE_LEFT) {this->buttons(_buttons & ~b);}
      bool isPressed(uint8_t b = MOUSE_LEFT) {if ((b & _buttons) > 0) {return true;}return false;}
      template<typename T> bool sendReport(T report) {
        return hid.SendReport(HID_REPORT_ID_MOUSE_s,&report,_type->report_size,50);
      }
      uint16_t _onGetDescriptor(uint8_t *buffer) override {
        memcpy(buffer,_type->report_descriptor,_type->descriptor_size);
        return _type->descriptor_size;
      }
      void move(int8_t x,int8_t y,int8_t wheel = 0,int8_t pan = 0) {
        hid_mouse_report_t report = {.buttons = _buttons,.x = x,.y = y,.wheel = wheel,.pan = pan};
        sendReport(report);
      }
      virtual void click(uint8_t b = MOUSE_LEFT) {_buttons = b;move(0, 0);_buttons = 0;move(0, 0);}
      virtual void buttons(uint8_t b) {if (b != _buttons) {_buttons = b;move(0, 0);}}
    protected:
      USBHID hid;
      uint8_t _buttons;
      HIDMouseType_t *_type;
  };
  class USBHIDRelativeMouse: public USBHIDMouseBase{
    public:
      USBHIDRelativeMouse(void): USBHIDMouseBase(&HIDMouseRel){}
      void move(int8_t x,int8_t y,int8_t wheel = 0,int8_t pan = 0) {
        hid_mouse_report_t report = {.buttons = _buttons,.x = x,.y = y,.wheel = wheel,.pan = pan};
        sendReport(report);
      }
      void click(uint8_t b = MOUSE_LEFT) {_buttons = b;move(0, 0);_buttons = 0;move(0, 0);}
      void buttons(uint8_t b) {if (b != _buttons) {_buttons = b;move(0, 0);}}
  };
  class USBHIDAbsoluteMouse: public USBHIDMouseBase{
    public:
      USBHIDAbsoluteMouse(void): USBHIDMouseBase(&HIDMouseAbs){}
      void move(int16_t x,int16_t y,int8_t wheel = 0,int8_t pan = 0) {
        hid_abs_mouse_report_t report;
        report.buttons = _buttons;report.x = _lastx = x;report.y = _lasty = y;report.wheel = wheel;report.pan = pan;
        sendReport(report);
        }
      void click(uint8_t b = MOUSE_LEFT) {_buttons = b;move(_lastx, _lasty);_buttons = 0;move(_lastx, _lasty);}
      void buttons(uint8_t b) {if (b != _buttons) {_buttons = b;move(_lastx, _lasty);}}
    private:
      int16_t _lastx = 0;
      int16_t _lasty = 0;
  };
  typedef USBHIDRelativeMouse USBHIDMouse;

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
  class USBHIDKeyboard : public USBHIDDevice, public Print {
    private:
      USBHID hid;
      KeyReports _keyReports;
      const uint8_t *_asciimap;
      bool shiftKeyReports;
    public:
      USBHIDKeyboard():hid(HID_ITF_PROTOCOL_KEYBOARD),_asciimap(CUSTOMKeyboardLayout_en_US),shiftKeyReports(false){
        static bool initialized=false;
        if(!initialized){initialized=true;memset(&_keyReports,0,sizeof(KeyReports));hid.addDevice(this,sizeof(report_descriptor));}
      }
      uint16_t _onGetDescriptor(uint8_t *dst) {memcpy(dst, report_descriptor, sizeof(report_descriptor));return sizeof(report_descriptor);}
      void begin(const uint8_t *layout = CUSTOMKeyboardLayout_en_US) {_asciimap = layout;hid.begin();}
      void end() {}
      void onEvent(esp_event_handler_t callback) {onEvent(ARDUINO_USB_HID_KEYBOARD_ANY_EVENT_s, callback);}
      void onEvent(arduino_usb_hid_keyboard_event_t_s event, esp_event_handler_t callback) {
        arduino_usb_event_handler_register_with(ARDUINO_USB_HID_KEYBOARDV_EVENTS, event, callback, this);
      }
      void _onOutput(uint8_t report_id, const uint8_t *buffer, uint16_t len) {
        if (report_id == HID_REPORT_ID_KEYBOARD_s) {
          arduino_usb_hid_keyboard_event_data_t_s p;p.leds = buffer[0];
          arduino_usb_event_post(ARDUINO_USB_HID_KEYBOARDV_EVENTS, ARDUINO_USB_HID_KEYBOARD_LED_EVENT_s, &p, sizeof(arduino_usb_hid_keyboard_event_data_t_s), portMAX_DELAY);
        }
      }
      void sendReport(KeyReports *keys) {
        hid_keyboard_report_t report;
        report.reserved = 0;
        report.modifier = keys->modifiers;
        memcpy(report.keycode, keys->keys, 6);
        hid.SendReport(HID_REPORT_ID_KEYBOARD_s, &report, sizeof(report),50);
      }
      void setShiftKeyReports(bool set) {shiftKeyReports = set;}
      size_t pressRaw(uint8_t k) {
        uint8_t i;
        if (k >= 0xE0 && k < 0xE8) {_keyReports.modifiers |= (1 << (k - 0xE0));} else if (k && k < 0xA5) {
          if (_keyReports.keys[0] != k && _keyReports.keys[1] != k && _keyReports.keys[2] != k && _keyReports.keys[3] != k && _keyReports.keys[4] != k&& _keyReports.keys[5] != k) {
            for (i = 0; i < 6; i++) {if (_keyReports.keys[i] == 0x00) {_keyReports.keys[i] = k;break;}}
            if (i == 6) {return 0;}
          }
        } else if (_keyReports.modifiers == 0) {return 0;}
        sendReport(&_keyReports);
        return 1;
      }
      size_t releaseRaw(uint8_t k) {
        uint8_t i;
        if (k >= 0xE0 && k < 0xE8) {_keyReports.modifiers &= ~(1 << (k - 0xE0));} else if (k && k < 0xA5) {
          for (i = 0; i < 6; i++) {if (0 != k && _keyReports.keys[i] == k) {_keyReports.keys[i] = 0x00;}}
        }
        sendReport(&_keyReports);
        return 1;
      }
      size_t press(uint8_t k) {
        if (k >= 0x88) {k = k - 0x88;} else if (k >= 0x80) {_keyReports.modifiers |= (1 << (k - 0x80));k = 0;} else {
          k = _asciimap[k];
          if (!k) {return 0;}
          if ((k & SHIFT) == SHIFT) { 
            if (shiftKeyReports) {pressRaw(HID_KEY_SHIFT_LEFT);} else {_keyReports.modifiers |= 0x02;}
            k &= ~SHIFT;
          }
          if ((k & ALT_GR) == ALT_GR) {_keyReports.modifiers |= 0x40;k &= ~ALT_GR;}
          if (k == ISO_REPLACEMENT) {k = ISO_KEY;}
        }
        return pressRaw(k);
      }
      size_t release(uint8_t k) {
        if (k >= 0x88) {k = k - 0x88;}else if (k >= 0x80) {_keyReports.modifiers &= ~(1 << (k - 0x80));k = 0;}else {
          k = _asciimap[k];
          if (!k) {return 0;}
          if ((k & SHIFT) == SHIFT) {
            if (shiftKeyReports) {releaseRaw(k & 0x7F); k = HID_KEY_SHIFT_LEFT;  }
            else {_keyReports.modifiers &= ~(0x02);k &= ~SHIFT;}
          }
          if ((k & ALT_GR) == ALT_GR) {_keyReports.modifiers &= ~(0x40); k &= ~ALT_GR;}
          if (k == ISO_REPLACEMENT) {k = ISO_KEY;}
        }
        return releaseRaw(k);
      }
      void releaseAll(void) {
        _keyReports.keys[0] = 0;_keyReports.keys[1] = 0;_keyReports.keys[2] = 0;_keyReports.keys[3] = 0;_keyReports.keys[4] = 0;_keyReports.keys[5] = 0;_keyReports.modifiers = 0;
        sendReport(&_keyReports);
      }
      size_t write(uint8_t c) {
        uint8_t p = press(c);
        release(c);
        return p;
      }
      size_t write(const uint8_t *buffer, size_t size) {
        size_t n = 0;
        while (size--) {if (*buffer != '\r') {if (write(*buffer)) {n++;} else {break;}}buffer++;}
        return n;
      }
      void sendPhysicalReport(uint8_t modifiers,uint8_t keycode) {
        KeyReports merged=_keyReports;
        merged.modifiers|=modifiers;
        if(keycode){for(int i=0;i<6;i++){if(merged.keys[i]==0){merged.keys[i]=keycode;break;}}}
        sendReport(&merged);
      }

  };

  const uint8_t HID_BACKSPACE=0x2A,HID_DELETE=0x4C,HID_LEFT=0x50,HID_DOWN=0x51,HID_UP= 0x52,HID_RIGHT=0x4F,HID_ENTER=0x28,HID_TAB=0x2B,HID_ESCAPE=0x29,HID_SPACE=0x2C,HID_MOD_CTRL=0x01,HID_MOD_SHIFT=0x02,HID_MOD_ALT=0x04;
  class KEYBOARDtab : public USBHIDDevice {
    public:
      m5::unit::UnitUnified Units;
      m5::unit::UnitTab5Keyboard unit;
      KEYBOARDtab(custom::USBHIDKeyboard& kb) : keyboard(kb) {}
      bool begin() {
        auto cfg=unit.config();
        cfg.mode=m5::unit::tab5_keyboard::Mode::HID;
        cfg.start_periodic=true;
        cfg.irq_pin=50;
        unit.config(cfg);
        Wire.end();
        Wire.begin(TAB5_KEYBOARD_SDA,TAB5_KEYBOARD_SCL,unit.component_config().clock);
        if(!Units.add(unit,Wire)||!Units.begin()){return false;}
        Serial.printf("Tab5 Keyboard OK - firmware: %02X\n",unit.firmwareVersion());
        unit.writeRgbMode(m5::unit::tab5_keyboard::RgbMode::Custom);unit.writeBrightness(20);unit.writeRgb(0,255,0,0);unit.writeRgb(1,0,0,255);
        return true;
      };
      void ledctrl(int index, int r, int g, int b){
        unit.writeRgb(index,r,g,b);
      };
      std::string key_name(const uint8_t keycode, const char c){
        switch (keycode) {
          case custom::HID_BACKSPACE: return "Backspace";
          case custom::HID_DELETE: return "Delete";
          case custom::HID_LEFT: return "Left";
          case custom::HID_RIGHT: return "Right";
          case custom::HID_UP: return "Up";
          case custom::HID_DOWN: return "Down";
          case custom::HID_ENTER: return "Enter";
          case custom::HID_TAB: return "Tab";
          case custom::HID_ESCAPE: return "Esc";
          case custom::HID_SPACE: return "Space";
          default:break;
        }
        if(c!=0&&std::isprint(static_cast<unsigned char>(c))){return std::string(1,c);}
        return m5::utility::formatString("kc=0x%02X",keycode);
      }
      void forward_hid_event(const m5::unit::tab5_keyboard::Event& evt){
        keyboard.sendPhysicalReport(evt.modifier,evt.hid.keycode);
      }
      void drain_keyboard_events(int A){
        if(A==0&&!unit.empty()){
          const auto evt=unit.oldest();if(evt.type==m5::unit::tab5_keyboard::EventType::Hid){forward_hid_event(evt);}unit.discard();
        }
        else if(A==1)while(!unit.empty()){
          const auto evt=unit.oldest();if(evt.type==m5::unit::tab5_keyboard::EventType::Hid){forward_hid_event(evt);}unit.discard();
        }
      }
    private:
      int8_t TAB5_KEYBOARD_SDA=0,TAB5_KEYBOARD_SCL=1;
      custom::USBHIDKeyboard& keyboard;
  };

  bool Mounted() {return tud_mounted();}
  bool hidReady() {return tud_hid_n_ready(0);}

  #endif /* CONFIG_TINYUSB_HID_ENABLED */
  #endif /* SOC_USB_OTG_SUPPORTED */

}

custom::USBHIDMouseBase ms1(&custom::HIDMouseRel);
custom::USBHIDKeyboard kbv;
custom::KEYBOARDtab kbp(kbv);

m5::touch_detail_t touchDetail;
LGFX_Button bts[7];
LGFX_Button btkk[60];
rg2 lreg2;

//optimise function
void psv(int A,int B,int C,int D,String* txt,int idx,const char* fmt,...){
  char buf[96];
  va_list ap;va_start(ap,fmt);vsnprintf(buf,sizeof(buf),fmt,ap);va_end(ap);txt[idx]=buf;
  int difa=A+(C/2),difb=B+(D/2);
  M5.Lcd.drawRect(A-1,B-1,C+2,D+2,BLACK);
  M5.Lcd.fillRect(A,B,C,D,WHITE); 
  M5.Display.drawString(txt[idx],difa,difb);
}
char getchattoui(int reg,int pos,int difa,int difb,int SX,int SY,int C, int D,int i){
  const char* charlist=nullptr;
  if(reg<0||reg>=3){return '0';}
  switch(reg){
    case 0:charlist=AZERTYUP;break;
    case 1:charlist=AZERTYDOWN;break;
    case 2:charlist=NUMBER;break;
  }
  if(!charlist){return '1';}
  int len=strlen(charlist);
  if(len==0){return '2';}
  if(pos<0||pos>=len){return '3';}

  static char buf[2];buf[0]=charlist[pos];buf[1]='\0';

  btkk[i].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,buf,SX,SY);
  return '4';
}
void buttons(int select, int A,int B,int C,int D,String E,int F){
  if(select==0){
    int difa=A+(C/2),difb=B+(D/2),SX=2,SY=2;
    if(F==1){bts[0].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"L CLICK",SX,SY);}
    if(F==2){bts[1].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"R CLICK",SX,SY);}
    if(F==3){bts[2].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"spd cursor",SX,SY);}
    if(F==4){bts[3].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"none",SX,SY);}
    if(F==5){bts[4].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"mouse md",SX,SY);}
    if(F==6){bts[5].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"ch-slp md",SX,SY);}
    if(F==7){bts[6].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"none",SX,SY);}
    bts[F-1].drawButton();
  }
  if(select==1){
    int difa=A+(C/2),difb=B+(D/2),SX=2,SY=2;
    for(int i=0;i<26;i++){int selected=i+1;if(F==selected){getchattoui(0,i,difa,difb,SX,SY,C,D,i);}}
    for(int i=26;i<36;i++){int selected=i+1;if(F==selected){getchattoui(2,i-26,difa,difb,SX,SY,C,D,i);}}
    if(F==37){btkk[36].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"supr",SX,SY);}
    if(F==38){btkk[37].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"back",SX,SY);}
    if(F==39){btkk[38].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"enter",SX,SY);}
    if(F==40){btkk[39].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"space",SX,SY);}
    if(F==41){btkk[40].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"tab",SX,SY);}
    if(F==42){btkk[41].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"ctrl",SX,SY);}
    if(F==43){btkk[42].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"alt",SX,SY);}
    if(F==44){btkk[43].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"alt_gr",SX,SY);}
    if(F==45){btkk[44].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"maj",SX,SY);}
    if(F==46){btkk[45].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"up",SX,SY);}
    if(F==47){btkk[46].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"down",SX,SY);}
    if(F==48){btkk[47].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"left",SX,SY);}
    if(F==49){btkk[48].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"right",SX,SY);}
    //11 restants
    btkk[F-1].drawButton();
  }
}

//middle brain
void mousepad(int reg,int x,int y){
  int xms=820,dxms=455,dxms2=50,yms=300,dyms=410;dxms=dxms-dxms2;
  int decs=xms+dxms2;
  int cx1=decs+(dxms/2),cy=yms+(dyms/2);
  bool st1=((x>decs)&&(x<(decs+dxms))),st2=((x>xms)&&(x<(xms+dxms2))),st3=((y>yms)&&(y<(yms+dyms)));
  bool ist=st1&&st3,ist2=st2&&st3;
  if(reg==0){
    M5.Lcd.drawRect(xms,yms,dxms2,dyms,BLACK);
    M5.Lcd.fillRect(xms+1,yms+1,dxms-2,dyms-2,WHITE);
    M5.Lcd.drawRect(decs,yms,dxms,dyms,BLACK);
    M5.Lcd.fillRect(cx1-42,cy-42,84,84,BLACK);
  }//mousepad(0,0,0);
  if(reg==1){
    onToutch[7]=ist;onToutch[8]=ist2;
    if(ist){padX=x;padY=y;}else{padX=-1;padY=-1;}
    if(ist2){scroll=y;}else{scroll=-1;}
  }//mousepad(1,x,y);
  if(reg==2){
    if(onToutch[7]){
      if(plX==-1){plX=padX;}int sx2=(padX-plX),lima=((decs+dxms)-(decs))/2;
      int outa=(kbtp==2)?(map((padX-cx1),-lima,lima,-8,8)):(0);plX=padX;
      if(plY==-1){plY=padY;}int sy2=(padY-plY),limb=((yms+dyms)-(yms))/2;
      int outb=(kbtp==2)?(map((padY-cy),-limb,limb,-8,8)):(0);plY=padY;
      int sx=outa+sx2,sy=outb+sy2;
      if(!((sx==0)&&(sy==0))){ms1.move(sx*sensy,sy*sensy,0,0);}
    }
    else if(onToutch[8]){
      if(lss==-1){lss=scroll;}int sy2=(scroll-lss),limb=((yms+dyms)-(yms))/2;
      int outb=(kbtp==2)?(map((scroll-cy),-limb,limb,-6,6)):(0);lss=scroll;
      int sy=outb+sy2;
      if(!(sy==0)){ms1.move(0,0,sy*sensy,0);}
    }
    else{plX=-1;plY=-1;lss=-1;}
  }//mousepad(2,0,0);
  if(reg==3){
    plX=-1;plY=-1;lss=-1;
    padX=0;padY=0;scroll=0;
    onToutch[7]=false;
    onToutch[8]=false;
  }//mousepad(3,0,0);
  
};
void keys(int reg,int k,bool mds){
  if(reg==0){
    char key2;
    uint8_t code3=0x00;
    if(k<26){key2=(majlock?('A'):('a'))+k;}
    if(k>=26){code3=lreg2.KP_1+(k-26);}
    if(code3!=0x00){kbv.write(code3);}else{kbv.write(key2);}
  }
  if(reg==1){
    uint8_t code1=0x00;
    switch(k){
      case 36:code1=lreg2.DELETE;break;
      case 37:code1=lreg2.BACKSPACE;break;
      case 38:code1=lreg2.ENTER;break;
      case 39:code1=lreg2.SPACE;break;
      case 40:code1=lreg2.TAB;break;
      case 41:code1=lreg2.LCTRL;break;
      case 42:code1=lreg2.LALT;break;
      case 43:code1=lreg2.RALT;break;
      case 44:code1=lreg2.LSHIFT;majlock=mds;break;
      case 45:code1=lreg2.UP;break;
      case 46:code1=lreg2.DOWN;break;
      case 47:code1=lreg2.LEFT;break;
      case 48:code1=lreg2.RIGHT;break;
    }
    if(code1!=0x00){if(mds){kbv.press(code1);}else{kbv.release(code1);}}
  }
}

//initial
void setup(){
  //init serial
  Serial.begin(115200);
  //init m5
  auto cfgm5 = M5.config();
  M5.begin(cfgm5);
  //init usb
  USB.begin();
  //init virtual mouse
  ms1.begin();
  //init virtual keyboard
  kbv.begin();
  //init m5 keyboard physique
  if(!kbp.begin()){Serial.println("Failed to begin UnitTab5Keyboard");kbfstt=0;}else{kbfstt=1;}
  //initscreen
  M5.Lcd.fillScreen(WHITE);
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(BLACK);
  M5.Display.setTextDatum(middle_center);
  buttons(0,0+(170*0),610,170,90,"",1);
  buttons(0,0+(170*1),610,170,90,"",2);
  buttons(0,820+(225*1),0+(100*0),225,100,"",3);
  //buttons(0,0+(225*0),0+(100*0),225,100,"",4);
  buttons(0,820+(225*1),0+(100*1),225,100,"",5);
  buttons(0,820+(225*1),0+(100*2),225,100,"",6);
  //buttons(0,0+(225*0),0+(100*0),225,100"",7);
  int XKB=5,YKB=5,indexkb=1;
  M5.Lcd.drawRect(XKB,YKB,1035,600,BLACK);
  int DXKB=103,DYKB=99,lim=36;
  for(int ia=0;ia<4;ia++){
    for(int ib=0;ib<10;ib++){
      buttons(1,XKB+(DXKB*ib),YKB+(DYKB*ia),DXKB,DYKB,"",indexkb);
      indexkb++;
      if((indexkb+-1)==lim){break;}
    }
    if((indexkb+-1)==lim){break;}
  }
  int sep=135;
  buttons(1,5+(sep*0),420,sep,90,"",indexkb);indexkb++;//supr
  buttons(1,5+(sep*1),420,sep,90,"",indexkb);indexkb++;//del
  buttons(1,5+(sep*2),420,sep,90,"",indexkb);indexkb++;//enter
  buttons(1,5+(sep*0),510,sep,90,"",indexkb);indexkb++;//space
  buttons(1,5+(sep*1),510,sep,90,"",indexkb);indexkb++;//tab
  buttons(1,400,610,100,90,"",indexkb);indexkb++;//ctrl
  buttons(1,500,610,100,90,"",indexkb);indexkb++;//alt
  buttons(1,600,610,100,90,"",indexkb);indexkb++;//altgr
  buttons(1,700,610,100,90,"",indexkb);indexkb++;//maj
  buttons(1,5+(sep*4),420,sep,90,"",indexkb);indexkb++;//up
  buttons(1,5+(sep*3),510,sep,90,"",indexkb);indexkb++;//down
  buttons(1,5+(sep*4),510,sep,90,"",indexkb);indexkb++;//left
  buttons(1,5+(sep*5),510,sep,90,"",indexkb);indexkb++;//right
  mousepad(0,0,0);
  //init data table ui
  for(int i=0;i<sizea;i++){txt[i]="";}
  //charge config
  M5.Power.setBatteryCharge(chargemode);
  M5.Power.setChargeCurrent(300);
  M5.Power.setChargeVoltage(8300);//max 8400mv => 8.4v
  M5.Power.setExtOutput(false,m5::ext_PA);
  M5.Power.setExtOutput(false,m5::ext_PB1);
  M5.Power.setExtOutput(false,m5::ext_PB2);
  M5.Power.setExtOutput(false,m5::ext_PC1);
  M5.Power.setExtOutput(false,m5::ext_PC2);
  M5.Power.setExtOutput(true,m5::ext_USB);
  M5.Power.setExtOutput(false,m5::ext_PWR485);
  M5.Power.setExtOutput(false,m5::ext_PWRCAN);
}

void loop(){
  //update
  M5.update();
  kbp.Units.update();
  //physical keyboard
  kbp.drain_keyboard_events(1);
  //brain toutch contact
  int touches=M5.Touch.getCount();
  if(touches==0){
    mousepad(3,0,0);
    if(lockR){ms1.release(custom::MOUSE_RIGHT);lockR=false;}
    if(lockL){ms1.release(custom::MOUSE_LEFT);lockL=false;}
    if(lockSS){lockSS=false;}
    if(lockMMD){lockMMD=false;}
    if(lockCharge){lockCharge=false;}
    for(int k=36;k<49;k++){if(lockKey[k]){keys(1,k,false);lockKey[k]=false;}}
  }
  for(int k=0;k<8;k++){onToutch[k]=false;}
  for(int k=0;k<49;k++){keyTouched[k]=false;}
  if(touches>0)for(int i=0;i<touches;i++){
    autosleep=0;
    auto t=M5.Touch.getDetail(i);
    int x=t.x,y=t.y;
    mousepad(1,x,y);
    for(int k=0;k<7;k++){if(bts[k].contains(x,y)){onToutch[k]=true;}}
    for(int k=0;k<49;k++){if(btkk[k].contains(x,y)){keyTouched[k]=true;}}
  }
  for(int k=0;k<49;k++){
    if(keyTouched[k]){if(!lockKey[k]){if(k<36){keys(0,k,false);}else{keys(1,k,true);}lockKey[k]=true;}}
    else{if(k<36){lockKey[k]=false;}else{if(lockKey[k]){keys(1,k,false);lockKey[k]=false;}}}
  }
  if(onToutch[0]){if(!lockL){ms1.press(custom::MOUSE_LEFT);lockL=true;}}else{if(lockL){ms1.release(custom::MOUSE_LEFT);lockL=false;}}
  if(onToutch[1]){if(!lockR){ms1.press(custom::MOUSE_RIGHT);lockR=true;}}else{if(lockR){ms1.release(custom::MOUSE_RIGHT);lockR=false;}}
  if(onToutch[2]){if(!lockSS){if(sensy==1){sensy=2;}else if(sensy==2){sensy=3;}else{sensy=1;}lockSS=true;}}else{if(lockSS){lockSS=false;}}
  if(onToutch[3]){}
  if(onToutch[4]){if(!lockMMD){kbtp=kbtp+1;if(kbtp>2){kbtp=1;}lockMMD=true;}}else{if(lockMMD){lockMMD=false;}}
  if(onToutch[5]){if(!lockCharge){chargemode=!chargemode;lockCharge=true;M5.Power.setBatteryCharge(chargemode);}}else{if(lockCharge){lockCharge=false;}}
  if(onToutch[6]){}
  mousepad(2,0,0);
  if(touches!=lasttoutch){lasttoutch=touches;}
  //cicle clock ui et status
  unsigned long now=millis();
  if(now-timer>=1900){
    timer=now;
    float bat=M5.Power.getBatteryLevel();
    int cons=M5.Power.getBatteryCurrent();
    int lda=map(bat,0,100,255,0),ldb=map(bat,0,100,0,255);
    bool tta=(cons>0),ttb=(cons<10),ttc=(cons>-5&&cons<20);
    int ld1[3]={0,0,0};
    ld1[0]=lda;
    ld1[1]=ldb;
    ld1info=chargemode?((ld1info==0)?1:0):0;
    if(ld1info==1){
      ld1[0]=tta?255:0;
      ld1[1]=ttb?255:0;
      ld1[2]=ttb?255:0;
      autosleep=(tta&&ttb&&ttc)?(autosleep+1):0;
    }
    kbp.ledctrl(0,ld1[0],ld1[1],ld1[2]);

    int ld2[3]={0,0,0};
    kbfstt=custom::hidReady()?2:custom::Mounted()?1:0;
    switch(kbfstt){
      case 0:ld2[0]=255;ld2[1]=0;ld2[2]=0;break;
      case 1:ld2[0]=255;ld2[1]=180;ld2[2]=0;break;
      case 2:ld2[0]=0;ld2[1]=255;ld2[2]=0;break;
      default:ld2[0]=0;ld2[1]=0;ld2[2]=0;break;
    }
    kbp.ledctrl(1,ld2[0],ld2[1],ld2[2]);

    psv(1,701,800,20,txt,0,"%.2f%% %dmA %dsec %S*%d",bat,cons,(remain-autosleep),((kbtp==1)?"normal":"mixed"),sensy);
  }
  if((autosleep-remain)<=0&&chargemode){M5.Power.powerOff();}
  //clock delay
  delay(4);
}
