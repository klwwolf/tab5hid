#include "Tab5USBHID.h"


#if SOC_USB_OTG_SUPPORTED
#if CONFIG_TINYUSB_HID_ENABLED
#include "esp32-hal-tinyusb.h"
#include "USB.h"
#include "esp_hid_common.h"
#define USB_HID_DEVICES_MAX 10
#include "CUSTOMKeyboardLayout.h"


ESP_EVENT_DEFINE_BASE(ARDUINO_USB_HID_EVENTS);
ESP_EVENT_DEFINE_BASE(ARDUINO_USB_HID_KEYBOARD_EVENTS);


esp_err_t arduino_usb_event_post(esp_event_base_t event_base, int32_t event_id, void *event_data, size_t event_data_size, TickType_t ticks_to_wait);
esp_err_t arduino_usb_event_handler_register_with(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler, void *event_handler_arg);
typedef struct {CUSTOMUSBHIDDevice *device;uint8_t reports_num;uint8_t *report_ids;} tinyusb_hid_device_t;
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


CUSTOMUSBHIDDevice *tinyusb_get_device_by_report_id(uint8_t report_id) {
  for (uint8_t i = 0; i < tinyusb_loaded_hid_devices_num; i++) {
    tinyusb_hid_device_t *device = &tinyusb_hid_devices[i];
    if (device->device && device->reports_num) {
      for (uint8_t r = 0; r < device->reports_num; r++) {
        if (report_id == device->report_ids[r]) {
          return device->device;
        }
      }
    }
  }
  return NULL;
}

//usbhid
static bool tinyusb_enable_hid_device(uint16_t descriptor_len, CUSTOMUSBHIDDevice *device) {
  if (tinyusb_hid_is_initialized) {
    log_e("TinyUSB HID has already started! Device not enabled");
    return false;
  }
  if (tinyusb_loaded_hid_devices_num >= USB_HID_DEVICES_MAX) {
    log_e("Maximum devices already enabled! Device not enabled");
    return false;
  }
  tinyusb_hid_device_descriptor_len += descriptor_len;
  tinyusb_hid_devices[tinyusb_loaded_hid_devices_num++].device = device;

  log_d("Device[%u] len: %u", tinyusb_loaded_hid_devices_num - 1, descriptor_len);
  return true;
}
static uint16_t tinyusb_on_get_feature(uint8_t report_id, uint8_t *buffer, uint16_t reqlen) {
  CUSTOMUSBHIDDevice *device = tinyusb_get_device_by_report_id(report_id);
  if (device) {
    return device->_onGetFeature(report_id, buffer, reqlen);
  }
  return 0;
}
static bool tinyusb_on_set_feature(uint8_t report_id, const uint8_t *buffer, uint16_t reqlen) {
  CUSTOMUSBHIDDevice *device = tinyusb_get_device_by_report_id(report_id);
  if (device) {
    device->_onSetFeature(report_id, buffer, reqlen);
    return true;
  }
  return false;
}
static bool tinyusb_on_set_output(uint8_t report_id, const uint8_t *buffer, uint16_t reqlen) {
  CUSTOMUSBHIDDevice *device = tinyusb_get_device_by_report_id(report_id);
  if (device) {
    device->_onOutput(report_id, buffer, reqlen);
    return true;
  }
  return false;
}
static uint16_t tinyusb_on_add_descriptor(uint8_t device_index, uint8_t *dst) {
  uint16_t res = 0;
  uint8_t report_id = 0, reports_num = 0;
  tinyusb_hid_device_t *device = &tinyusb_hid_devices[device_index];
  if (device->device) {
    res = device->device->_onGetDescriptor(dst);
    if (res) {

      esp_hid_report_map_t *hid_report_map = esp_hid_parse_report_map(dst, res);
      if (hid_report_map) {
        if (device->report_ids) {
          free(device->report_ids);
        }
        device->reports_num = hid_report_map->reports_len;
        device->report_ids = (uint8_t *)malloc(device->reports_num);
        memset(device->report_ids, 0, device->reports_num);
        reports_num = device->reports_num;

        for (uint8_t i = 0; i < device->reports_num; i++) {
          if (hid_report_map->reports[i].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
            report_id = hid_report_map->reports[i].report_id;
            for (uint8_t r = 0; r < device->reports_num; r++) {
              if (!report_id) {break;}
              else if (report_id == device->report_ids[r]) {reports_num--;break;}
              else if (!device->report_ids[r]) {device->report_ids[r] = report_id;break;}
            }
          } else {reports_num--;}
        }
        device->reports_num = reports_num;
        esp_hid_free_report_map(hid_report_map);
      }
    }
  }
  return res;
}
static bool tinyusb_load_enabled_hid_devices() {
  if (tinyusb_hid_device_descriptor != NULL) {
    return true;
  }
  tinyusb_hid_device_descriptor = (uint8_t *)malloc(tinyusb_hid_device_descriptor_len);
  if (tinyusb_hid_device_descriptor == NULL) {
    log_e("HID Descriptor Malloc Failed");
    return false;
  }
  uint8_t *dst = tinyusb_hid_device_descriptor;

  for (uint8_t i = 0; i < tinyusb_loaded_hid_devices_num; i++) {
    uint16_t len = tinyusb_on_add_descriptor(i, dst);
    if (!len) {
      break;
    } else {
      dst += len;
    }
  }

  esp_hid_report_map_t *hid_report_map = esp_hid_parse_report_map(tinyusb_hid_device_descriptor, tinyusb_hid_device_descriptor_len);
  if (hid_report_map) {
    log_d("Loaded HID Descriptor with the following reports:");
    for (uint8_t i = 0; i < hid_report_map->reports_len; i++) {
      if (hid_report_map->reports[i].protocol_mode == ESP_HID_PROTOCOL_MODE_REPORT) {
        log_d(
          "  ID: %3u, Type: %7s, Size: %2u, Usage: %8s", hid_report_map->reports[i].report_id, esp_hid_report_type_str(hid_report_map->reports[i].report_type),
          hid_report_map->reports[i].value_len, esp_hid_usage_str(hid_report_map->reports[i].usage)
        );
      }
    }
    esp_hid_free_report_map(hid_report_map);
  } else {
    log_e("Failed to parse the hid report descriptor!");
    return false;
  }

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
  arduino_usb_hid_event_data_t p;
  p.instance = instance;
  p.set_protocol.protocol = protocol;
  arduino_usb_event_post(ARDUINO_USB_HID_EVENTS, ARDUINO_USB_HID_SET_PROTOCOL_EVENT, &p, sizeof(arduino_usb_hid_event_data_t), portMAX_DELAY);
}
bool tud_hid_set_idle_cb(uint8_t instance, uint8_t idle_rate) {
  log_v("instance: %u, idle_rate:%u", instance, idle_rate);
  arduino_usb_hid_event_data_t p;
  p.instance = instance;
  p.set_idle.idle_rate = idle_rate;
  arduino_usb_event_post(ARDUINO_USB_HID_EVENTS, ARDUINO_USB_HID_SET_IDLE_EVENT, &p, sizeof(arduino_usb_hid_event_data_t), portMAX_DELAY);
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
//usbhidmouse
static const uint8_t abs_mouse_report_descriptor[] = {TUD_HID_REPORT_DESC_ABSMOUSE(HID_REPORT_ID(HID_REPORT_ID_MOUSE))};
HIDMouseType_t HIDMouseAbs = {HID_MOUSE_ABSOLUTE, abs_mouse_report_descriptor, sizeof(abs_mouse_report_descriptor), sizeof(hid_abs_mouse_report_t)};
static const uint8_t rel_mouse_report_descriptor[] = {TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_REPORT_ID_MOUSE))};
HIDMouseType_t HIDMouseRel = {HID_MOUSE_RELATIVE, rel_mouse_report_descriptor, sizeof(rel_mouse_report_descriptor), sizeof(hid_mouse_report_t)};
//usbhidkeyboard
esp_err_t arduino_usb_event_post(esp_event_base_t event_base, int32_t event_id, void *event_data, size_t event_data_size, TickType_t ticks_to_wait);
esp_err_t arduino_usb_event_handler_register_with(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler, void *event_handler_arg);
static const uint8_t report_descriptor[] = {TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_REPORT_ID_KEYBOARD))};

CUSTOMUSBHID::CUSTOMUSBHID(hid_interface_protocol_enum_t itf_protocol) {
  if (!tinyusb_hid_devices_is_initialized) {
    tinyusb_hid_devices_is_initialized = true;
    for (uint8_t i = 0; i < USB_HID_DEVICES_MAX; i++) {
      memset(&tinyusb_hid_devices[i], 0, sizeof(tinyusb_hid_device_t));
    }
    tinyusb_hid_devices_num = 0;
    tinyusb_interface_protocol = itf_protocol;
    tinyusb_enable_interface2(USB_INTERFACE_HID, TUD_HID_INOUT_DESC_LEN, tusb_hid_load_descriptor, itf_protocol == HID_ITF_PROTOCOL_KEYBOARD);
  }
}
void CUSTOMUSBHID::begin() {
  if (tinyusb_hid_device_input_sem == NULL) {tinyusb_hid_device_input_sem = xSemaphoreCreateBinary();}
  if (tinyusb_hid_device_input_mutex == NULL) {tinyusb_hid_device_input_mutex = xSemaphoreCreateMutex();}
}
void CUSTOMUSBHID::end() {
  if (tinyusb_hid_device_input_sem != NULL) {vSemaphoreDelete(tinyusb_hid_device_input_sem);tinyusb_hid_device_input_sem = NULL;}
  if (tinyusb_hid_device_input_mutex != NULL) {vSemaphoreDelete(tinyusb_hid_device_input_mutex);tinyusb_hid_device_input_mutex = NULL;}
}
bool CUSTOMUSBHID::ready(void) {return tud_hid_n_ready(0);}
bool CUSTOMUSBHID::SendReport(uint8_t id, const void *data, size_t len, uint32_t timeout_ms) {
  if (!tinyusb_hid_device_input_sem || !tinyusb_hid_device_input_mutex) {
    log_e("TX Semaphore is NULL. You must call CUSTOMUSBHID::begin() before you can send reports");
    return false;
  }
  if (xSemaphoreTake(tinyusb_hid_device_input_mutex, timeout_ms / portTICK_PERIOD_MS) != pdTRUE) {
    log_e("report %u mutex failed", id);
    return false;
  }
  uint8_t effective_id = ((tinyusb_interface_protocol != HID_ITF_PROTOCOL_NONE) && (tud_hid_n_get_protocol(0) == HID_PROTOCOL_BOOT)) ? 0 : id;
  bool res = ready();
  if (!res) {log_e("not ready");} else {
    xSemaphoreTake(tinyusb_hid_device_input_sem, 0);
    res = tud_hid_n_report(0, effective_id, data, len);
    if (!res) {log_e("report %u failed", id);} else {
      if (xSemaphoreTake(tinyusb_hid_device_input_sem, timeout_ms / portTICK_PERIOD_MS) != pdTRUE) {
        log_e("report %u wait failed", id);
        res = false;
      }
    }
  }

  xSemaphoreGive(tinyusb_hid_device_input_mutex);
  return res;
}
bool CUSTOMUSBHID::addDevice(CUSTOMUSBHIDDevice *device, uint16_t descriptor_len) {
  if (device && tinyusb_loaded_hid_devices_num < USB_HID_DEVICES_MAX) {
    if (!tinyusb_enable_hid_device(descriptor_len, device)) {return false;}
    return true;
  }
  return false;
}
void CUSTOMUSBHID::onEvent(esp_event_handler_t callback) {onEvent(ARDUINO_USB_HID_ANY_EVENT, callback);}
void CUSTOMUSBHID::onEvent(arduino_usb_hid_event_t event, esp_event_handler_t callback) {arduino_usb_event_handler_register_with(ARDUINO_USB_HID_EVENTS, event, callback, this);}
CUSTOMUSBHIDMouseBase::CUSTOMUSBHIDMouseBase(HIDMouseType_t *type) : hid(), _buttons(0), _type(type) {static bool initialized = false;if (!initialized) {initialized = true;hid.addDevice(this, _type->descriptor_size);}};
uint16_t CUSTOMUSBHIDMouseBase::_onGetDescriptor(uint8_t *dst) {memcpy(dst, _type->report_descriptor, _type->descriptor_size);return _type->descriptor_size;}
void CUSTOMUSBHIDMouseBase::buttons(uint8_t b) {if (b != _buttons) {_buttons = b;}}
void CUSTOMUSBHIDMouseBase::begin() {hid.begin();}
void CUSTOMUSBHIDMouseBase::end() {}
void CUSTOMUSBHIDMouseBase::press(uint8_t b) {this->buttons(_buttons | b);}
void CUSTOMUSBHIDMouseBase::release(uint8_t b) {this->buttons(_buttons & ~b);}
bool CUSTOMUSBHIDMouseBase::isPressed(uint8_t b) {if ((b & _buttons) > 0) {return true;}return false;}
void CUSTOMUSBHIDAbsoluteMouse::move(int16_t x, int16_t y, int8_t wheel, int8_t pan) {hid_abs_mouse_report_t report;report.buttons = _buttons;report.x = _lastx = x;report.y = _lasty = y;report.wheel = wheel;report.pan = pan;sendReport(report);}
void CUSTOMUSBHIDAbsoluteMouse::click(uint8_t b) {_buttons = b;move(_lastx, _lasty);_buttons = 0;move(_lastx, _lasty);}
void CUSTOMUSBHIDAbsoluteMouse::buttons(uint8_t b) {if (b != _buttons) {_buttons = b;move(_lastx, _lasty);}}
void CUSTOMUSBHIDRelativeMouse::move(int8_t x, int8_t y, int8_t wheel, int8_t pan) {hid_mouse_report_t report = {.buttons = _buttons, .x = x, .y = y, .wheel = wheel, .pan = pan};sendReport(report);}
void CUSTOMUSBHIDRelativeMouse::click(uint8_t b) {_buttons = b;move(0, 0);_buttons = 0;move(0, 0);}
void CUSTOMUSBHIDRelativeMouse::buttons(uint8_t b) {if (b != _buttons) {_buttons = b;move(0, 0);}}

CUSTOMUSBHIDKeyboard::CUSTOMUSBHIDKeyboard() : hid(HID_ITF_PROTOCOL_KEYBOARD), _asciimap(CUSTOMKeyboardLayout_en_US), shiftKeyReports(false) {
  static bool initialized = false;if (!initialized) {initialized = true;memset(&_keyReport, 0, sizeof(KeyReport));hid.addDevice(this, sizeof(report_descriptor));}
}
uint16_t CUSTOMUSBHIDKeyboard::_onGetDescriptor(uint8_t *dst) {memcpy(dst, report_descriptor, sizeof(report_descriptor));return sizeof(report_descriptor);}
void CUSTOMUSBHIDKeyboard::begin(const uint8_t *layout) {_asciimap = layout;hid.begin();}
void CUSTOMUSBHIDKeyboard::end() {}
void CUSTOMUSBHIDKeyboard::onEvent(esp_event_handler_t callback) {onEvent(ARDUINO_USB_HID_KEYBOARD_ANY_EVENT, callback);}
void CUSTOMUSBHIDKeyboard::onEvent(arduino_usb_hid_keyboard_event_t event, esp_event_handler_t callback) {arduino_usb_event_handler_register_with(ARDUINO_USB_HID_KEYBOARD_EVENTS, event, callback, this);}
void CUSTOMUSBHIDKeyboard::_onOutput(uint8_t report_id, const uint8_t *buffer, uint16_t len) {
  if (report_id == HID_REPORT_ID_KEYBOARD) {
    arduino_usb_hid_keyboard_event_data_t p;p.leds = buffer[0];
    arduino_usb_event_post(ARDUINO_USB_HID_KEYBOARD_EVENTS, ARDUINO_USB_HID_KEYBOARD_LED_EVENT, &p, sizeof(arduino_usb_hid_keyboard_event_data_t), portMAX_DELAY);
  }
}
void CUSTOMUSBHIDKeyboard::sendReport(KeyReport *keys) {
  hid_keyboard_report_t report;
  report.reserved = 0;
  report.modifier = keys->modifiers;
  memcpy(report.keycode, keys->keys, 6);
  hid.SendReport(HID_REPORT_ID_KEYBOARD, &report, sizeof(report));
}
void CUSTOMUSBHIDKeyboard::setShiftKeyReports(bool set) {shiftKeyReports = set;}
size_t CUSTOMUSBHIDKeyboard::pressRaw(uint8_t k) {
  uint8_t i;
  if (k >= 0xE0 && k < 0xE8) {_keyReport.modifiers |= (1 << (k - 0xE0));} else if (k && k < 0xA5) {
    if (_keyReport.keys[0] != k && _keyReport.keys[1] != k && _keyReport.keys[2] != k && _keyReport.keys[3] != k && _keyReport.keys[4] != k&& _keyReport.keys[5] != k) {
      for (i = 0; i < 6; i++) {if (_keyReport.keys[i] == 0x00) {_keyReport.keys[i] = k;break;}}
      if (i == 6) {return 0;}
    }
  } else if (_keyReport.modifiers == 0) {return 0;}
  sendReport(&_keyReport);
  return 1;
}
size_t CUSTOMUSBHIDKeyboard::releaseRaw(uint8_t k) {
  uint8_t i;
  if (k >= 0xE0 && k < 0xE8) {_keyReport.modifiers &= ~(1 << (k - 0xE0));} else if (k && k < 0xA5) {
    for (i = 0; i < 6; i++) {if (0 != k && _keyReport.keys[i] == k) {_keyReport.keys[i] = 0x00;}}
  }
  sendReport(&_keyReport);
  return 1;
}
size_t CUSTOMUSBHIDKeyboard::press(uint8_t k) {
  if (k >= 0x88) {k = k - 0x88;} else if (k >= 0x80) {_keyReport.modifiers |= (1 << (k - 0x80));k = 0;} else {
    k = _asciimap[k];
    if (!k) {return 0;}
    if ((k & SHIFT) == SHIFT) { 
      if (shiftKeyReports) {pressRaw(HID_KEY_SHIFT_LEFT);} else {_keyReport.modifiers |= 0x02;}
      k &= ~SHIFT;
    }
    if ((k & ALT_GR) == ALT_GR) {_keyReport.modifiers |= 0x40;k &= ~ALT_GR;}
    if (k == ISO_REPLACEMENT) {k = ISO_KEY;}
  }
  return pressRaw(k);
}
size_t CUSTOMUSBHIDKeyboard::release(uint8_t k) {
  if (k >= 0x88) {k = k - 0x88;}else if (k >= 0x80) {_keyReport.modifiers &= ~(1 << (k - 0x80));k = 0;}else {
    k = _asciimap[k];
    if (!k) {return 0;}
    if ((k & SHIFT) == SHIFT) {
      if (shiftKeyReports) {releaseRaw(k & 0x7F); k = HID_KEY_SHIFT_LEFT;  }
      else {_keyReport.modifiers &= ~(0x02);k &= ~SHIFT;}
    }
    if ((k & ALT_GR) == ALT_GR) {_keyReport.modifiers &= ~(0x40); k &= ~ALT_GR;}
    if (k == ISO_REPLACEMENT) {k = ISO_KEY;}
  }
  return releaseRaw(k);
}
void CUSTOMUSBHIDKeyboard::releaseAll(void) {_keyReport.keys[0] = 0;_keyReport.keys[1] = 0;_keyReport.keys[2] = 0;_keyReport.keys[3] = 0;_keyReport.keys[4] = 0;_keyReport.keys[5] = 0;_keyReport.modifiers = 0;sendReport(&_keyReport);}
size_t CUSTOMUSBHIDKeyboard::write(uint8_t c) {uint8_t p = press(c);release(c);return p;}
size_t CUSTOMUSBHIDKeyboard::write(const uint8_t *buffer, size_t size) {size_t n = 0;while (size--) {if (*buffer != '\r') {if (write(*buffer)) {n++;} else {break;}}buffer++;}return n;}

#endif
#endif