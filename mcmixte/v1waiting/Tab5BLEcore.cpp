#include "../Tab5BLEcore.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEHIDDevice.h>
#include <BLESecurity.h>
#include <HIDTypes.h>

Tab5BLECore* Tab5BLECore::_instance=nullptr;

static const uint8_t REPORT_MAP_COMBO_OLD[] = {
  // ===== KEYBOARD (ID 1) =====
  0x05,0x01,     0x09,0x06,     0xA1,0x01,     0x85,0x01,
  0x75,0x01,     0x95,0x08,     
  0x05,0x07,     0x19,0xE0,     0x29,0xE7,
  0x15,0x00,     0x25,0x01,
  0x81,0x02,      // 8 modifiers (CTRL, SHIFT, ALT, GUI...)
  0x75,0x08,     0x95,0x01,     0x81,0x01,      // 🔥 RESERVED (1 byte constant)
  0x75,0x08,     0x95,0x06,     0x05,0x07,     0x19,0x00,     0x29,0x65,
  0x15,0x00,     0x25,0x65,     0x81,0x00,      // 6 touches simultanées
  0xC0,
  // ===== MOUSE (ID 2) =====
  0x05,0x01,     0x09,0x02,     0xA1,0x01,     0x85,0x02,     0x09,0x01,     0xA1,0x00,
  0x05,0x09,     0x19,0x01,     0x29,0x05,     0x15,0x00,     0x25,0x01,
  0x75,0x01,     0x95,0x05,     0x81,0x02,
  0x75,0x03,     0x95,0x01,     0x81,0x03,
  0x05,0x01,     0x09,0x30,     0x09,0x31,     0x09,0x38,
  0x15,0x81,     0x25,0x7F,     0x75,0x08,     0x95,0x03,     0x81,0x06,
  0x05,0x0C,     0x0A,0x38,0x02,
  0x15,0x81,     0x25,0x7F,     0x75,0x08,     0x95,0x01,     0x81,0x06,
  0xC0,     0xC0
};

static const uint8_t REPORT_MAP_COMBO_TEST[] = {
  // ===== KEYBOARD (ID 1) =====
  0x05,0x01,     0x09,0x06,     0xA1,0x01,     0x85,0x01,
  0x75,0x01,     0x95,0x08,     
  0x05,0x07,     0x19,0xE0,     0x29,0xE7,
  0x15,0x00,     0x25,0x01,
  0x81,0x02,      // 8 modifiers (CTRL, SHIFT, ALT, GUI...)
  0x75,0x08,     0x95,0x01,     0x81,0x01,      // 🔥 RESERVED (1 byte constant)
  0x75,0x08,     0x95,0x06,     0x05,0x07,     0x19,0x00,     0x29,0x65,
  0x15,0x00,     0x25,0x65,     0x81,0x00,      // 6 touches simultanées
  0xC0,

  // ===== MOUSE (ID 2) =====
  0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,        // USAGE (Mouse)
  0xa1, 0x01,        // COLLECTION (Application)
  0x85, 0x02,        //report ID
  0x09, 0x01,        //   USAGE (Pointer)
  0xa1, 0x00,        //   COLLECTION (Physical)
  0x05, 0x09,        //     USAGE_PAGE (Button)
  0x19, 0x01,        //     USAGE_MINIMUM (Button 1)
  0x29, 0x05,        //     USAGE_MAXIMUM (Button 5)
  0x15, 0x00,        //     LOGICAL_MINIMUM (0)
  0x25, 0x01,        //     LOGICAL_MAXIMUM (1)
  0x95, 0x05,        //     REPORT_COUNT (5)
  0x75, 0x01,        //     REPORT_SIZE (1)
  0x81, 0x02,        //     INPUT (Data,Var,Abs)
  0x95, 0x01,        //     REPORT_COUNT (1)
  0x75, 0x03,        //     REPORT_SIZE (3)
  0x81, 0x03,        //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,        //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,        //     USAGE (X)
  0x09, 0x31,        //     USAGE (Y)
  0x15, 0x81,        //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,        //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,        //     REPORT_SIZE (8)
  0x95, 0x02,        //     REPORT_COUNT (2)
  0x81, 0x06,        //     INPUT (Data,Var,Rel)
  0x09, 0x38,        //     USAGE (Wheel)
  0x15, 0x81,        //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,        //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,        //     REPORT_SIZE (8)
  0x95, 0x01,        //     REPORT_COUNT (1)
  0x81, 0x06,        //     INPUT (Data,Var,Rel)
  0xc0,              //   END_COLLECTION
  0xc0               // END_COLLECTION
};
static const uint8_t REPORT_MAP_COMBO_TEST2[] = {
  // ===== MOUSE (ID 2) =====
  0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,        // USAGE (Mouse)
  0xa1, 0x01,        // COLLECTION (Application)
  0x85, 0x02,        //report ID
  0x09, 0x01,        //   USAGE (Pointer)
  0xa1, 0x00,        //   COLLECTION (Physical)
  0x05, 0x09,        //     USAGE_PAGE (Button)
  0x19, 0x01,        //     USAGE_MINIMUM (Button 1)
  0x29, 0x05,        //     USAGE_MAXIMUM (Button 5)
  0x15, 0x00,        //     LOGICAL_MINIMUM (0)
  0x25, 0x01,        //     LOGICAL_MAXIMUM (1)
  0x95, 0x05,        //     REPORT_COUNT (5)
  0x75, 0x01,        //     REPORT_SIZE (1)
  0x81, 0x02,        //     INPUT (Data,Var,Abs)
  0x95, 0x01,        //     REPORT_COUNT (1)
  0x75, 0x03,        //     REPORT_SIZE (3)
  0x81, 0x03,        //     INPUT (Cnst,Var,Abs)
  0x05, 0x01,        //     USAGE_PAGE (Generic Desktop)
  0x09, 0x30,        //     USAGE (X)
  0x09, 0x31,        //     USAGE (Y)
  0x15, 0x81,        //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,        //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,        //     REPORT_SIZE (8)
  0x95, 0x02,        //     REPORT_COUNT (2)
  0x81, 0x06,        //     INPUT (Data,Var,Rel)
  0x09, 0x38,        //     USAGE (Wheel)
  0x15, 0x81,        //     LOGICAL_MINIMUM (-127)
  0x25, 0x7f,        //     LOGICAL_MAXIMUM (127)
  0x75, 0x08,        //     REPORT_SIZE (8)
  0x95, 0x01,        //     REPORT_COUNT (1)
  0x81, 0x06,        //     INPUT (Data,Var,Rel)
  0xc0,              //   END_COLLECTION
  0xc0,               // END_COLLECTION
  // ===== KEYBOARD (ID 1) =====
  0x05,0x01,     0x09,0x06,     0xA1,0x01,     0x85,0x01,
  0x75,0x01,     0x95,0x08,     
  0x05,0x07,     0x19,0xE0,     0x29,0xE7,
  0x15,0x00,     0x25,0x01,
  0x81,0x02,      // 8 modifiers (CTRL, SHIFT, ALT, GUI...)
  0x75,0x08,     0x95,0x01,     0x81,0x01,      // 🔥 RESERVED (1 byte constant)
  0x75,0x08,     0x95,0x06,     0x05,0x07,     0x19,0x00,     0x29,0x65,
  0x15,0x00,     0x25,0x65,     0x81,0x00,      // 6 touches simultanées
  0xC0
};

class ServerCallbackscore : public BLEServerCallbacks {
  void onConnect(BLEServer* s) override {
    printf("[SERVER CONNECT - GENERIC]\n");
    if(Tab5BLECore::_instance){Tab5BLECore::_instance->_connId = s->getConnId();printf("[SERVER] connId=%u\n", s->getConnId());Tab5BLECore::_instance->handleConnect();}
  }
  void onDisconnect(BLEServer* s) override {printf("[SERVER DISCONNECT - GENERIC]\n");if(Tab5BLECore::_instance){Tab5BLECore::_instance->handleDisconnect();}}
  #if defined(CONFIG_NIMBLE_ENABLED)
    void onConnect(BLEServer* s, ble_gap_conn_desc* desc) override {
      printf("[SERVER CONNECT - NIMBLE]\n");
      if(desc){
        uint16_t mtu = s->getPeerMTU(s->getConnId());
        printf("conn_handle=%u | interval=%u | latency=%u | timeout=%u | mtu=%u\n",desc->conn_handle,desc->conn_itvl,desc->conn_latency,desc->supervision_timeout,mtu);
      }
      if(Tab5BLECore::_instance){Tab5BLECore::_instance->_connId = s->getConnId();printf("[SERVER] connId=%u\n", s->getConnId());Tab5BLECore::_instance->handleConnect();}
    }
    void onDisconnect(BLEServer* s, ble_gap_conn_desc* desc) override {
      printf("[SERVER DISCONNECT - NIMBLE]\n");
      if(desc){printf("conn_handle=%u\n",desc->conn_handle);}
      if(Tab5BLECore::_instance){Tab5BLECore::_instance->handleDisconnect();}
    }
    void onMtuChanged(BLEServer* s, ble_gap_conn_desc* desc, uint16_t mtu) override {printf("[SERVER MTU] %u\n", mtu);}
    void onConnParamsUpdate(uint16_t conn_handle, uint16_t interval, uint16_t latency, uint16_t timeout, uint8_t status) override {
      printf("[SERVER PARAM UPDATE] handle=%u interval=%u latency=%u timeout=%u status=%u\n",conn_handle, interval, latency, timeout, status);
    }
  #endif
  #if defined(CONFIG_BLUEDROID_ENABLED)
    void onConnect(BLEServer* s, esp_ble_gatts_cb_param_t* param) override {
      printf("[SERVER CONNECT - BLUEDROID]\n");
      if(Tab5BLECore::_instance){
        Tab5BLECore::_instance->_connId = s->getConnId();
        printf("[SERVER] connId=%u\n", s->getConnId());
        Tab5BLECore::_instance->handleConnect();
      }
    }
    void onDisconnect(BLEServer* s, esp_ble_gatts_cb_param_t* param) override {
      printf("[SERVER DISCONNECT - BLUEDROID]\n");
      if(Tab5BLECore::_instance){Tab5BLECore::_instance->handleDisconnect();}
    }
    void onMtuChanged(BLEServer* s, esp_ble_gatts_cb_param_t* param) override {printf("[SERVER MTU - BLUEDROID]\n");}
    void onConnParamsUpdate(esp_bd_addr_t remote_bda, uint16_t interval, uint16_t latency, uint16_t timeout, esp_bt_status_t status) override {
      printf("[SERVER PARAM UPDATE - BLUEDROID] interval=%u latency=%u timeout=%u status=%d\n",interval, latency, timeout, status);
    }
  #endif
};
class MouseCallbacks : public BLECharacteristicCallbacks {
  void onSubscribe(BLECharacteristic* c, ble_gap_conn_desc* desc, uint16_t subValue) override {
    printf("[MOUSE SUBSCRIBE] value=%u", subValue);
    if(subValue == 0){printf(" (UNSUBSCRIBE)");}
    else if(subValue == 1){printf(" (NOTIFY ENABLED)");}
    else if(subValue == 2){printf(" (INDICATE ENABLED)");}
    else if(subValue == 3){printf(" (NOTIFY + INDICATE)");}
    if(desc){printf(" | conn_id=%u | interval=%u | latency=%u",desc->conn_handle,desc->conn_itvl,desc->conn_latency);}
    printf("\n");
  }
  void onStatus(BLECharacteristic* c, Status s, uint32_t code) override {
    printf("[MOUSE STATUS] ");
    switch(s){
      case Status::SUCCESS_NOTIFY:printf("SUCCESS_NOTIFY");break;
      case Status::SUCCESS_INDICATE:printf("SUCCESS_INDICATE");break;
      case Status::ERROR_INDICATE_DISABLED:printf("ERROR_INDICATE_DISABLED");break;
      case Status::ERROR_NOTIFY_DISABLED:printf("ERROR_NOTIFY_DISABLED");break;
      case Status::ERROR_GATT:printf("ERROR_GATT");break;
      case Status::ERROR_NO_CLIENT:printf("ERROR_NO_CLIENT");break;
      case Status::ERROR_NO_SUBSCRIBER:printf("ERROR_NO_SUBSCRIBER");break;
      case Status::ERROR_INDICATE_TIMEOUT:printf("ERROR_INDICATE_TIMEOUT");break;
      case Status::ERROR_INDICATE_FAILURE:printf("ERROR_INDICATE_FAILURE");break;
      default:printf("UNKNOWN(%d)", (int)s);break;
    }
    printf(" | code=%lu\n", (unsigned long)code);
  }
  void onRead(BLECharacteristic* c) override {printf("[MOUSE READ]\n");}
  void onWrite(BLECharacteristic* c) override {
    String val = c->getValue();
    printf("[MOUSE WRITE] size=%d | ", (int)val.length());
    for(size_t i=0;i<val.length();i++){printf("%02X ", (uint8_t)val[i]);}
    printf("\n");
  }
};
class KeyboardCallbacks : public BLECharacteristicCallbacks {
  void onSubscribe(BLECharacteristic* c, ble_gap_conn_desc* desc, uint16_t subValue) override {
    printf("[KB SUBSCRIBE] value=%u", subValue);
    if(subValue == 0){printf(" (UNSUBSCRIBE)");}
    else if(subValue == 1){printf(" (NOTIFY ENABLED)");}
    else if(subValue == 2){printf(" (INDICATE ENABLED)");}
    else if(subValue == 3){printf(" (NOTIFY + INDICATE)");}
    if(desc){printf(" | conn_id=%u | interval=%u | latency=%u",desc->conn_handle,desc->conn_itvl,desc->conn_latency);
    }

    printf("\n");
  }
  void onStatus(BLECharacteristic* c, Status s, uint32_t code) override {
    printf("[KB STATUS] ");
    switch(s){
      case Status::SUCCESS_NOTIFY:printf("SUCCESS_NOTIFY");break;
      case Status::SUCCESS_INDICATE:printf("SUCCESS_INDICATE");break;
      case Status::ERROR_INDICATE_DISABLED:printf("ERROR_INDICATE_DISABLED");break;
      case Status::ERROR_NOTIFY_DISABLED:printf("ERROR_NOTIFY_DISABLED");break;
      case Status::ERROR_GATT:printf("ERROR_GATT");break;
      case Status::ERROR_NO_CLIENT:printf("ERROR_NO_CLIENT");break;
      case Status::ERROR_NO_SUBSCRIBER:printf("ERROR_NO_SUBSCRIBER");break;
      case Status::ERROR_INDICATE_TIMEOUT:printf("ERROR_INDICATE_TIMEOUT");break;
      case Status::ERROR_INDICATE_FAILURE:printf("ERROR_INDICATE_FAILURE");break;
      default:printf("UNKNOWN(%d)", (int)s);break;
    }
    printf(" | code=%lu\n", (unsigned long)code);
  }
  void onRead(BLECharacteristic* c) override {printf("[KB READ]\n");}
  void onWrite(BLECharacteristic* c) override {
    String val = c->getValue();
    printf("[KB WRITE] size=%d | ", (int)val.length());
    for(size_t i=0;i<val.length();i++){printf("%02X ", (uint8_t)val[i]);}
    printf("\n");
  }
};

//begin
Tab5BLECore::Tab5BLECore(uint16_t vid,uint16_t pid,uint16_t ver){
  _vid=vid;_pid=pid;_ver=ver;
  _hidDevice=nullptr;_advertising=nullptr;_server=nullptr;
  _inputKeyboard=nullptr;_inputMouse=nullptr;_inputGeneric=nullptr;
  _connected=false;_begun=false;_connId = 0xFFFF;
  _gene=false;_unified=false;
  memset(&_kreport,0,sizeof(_kreport));
  memset(&_mreport,0,sizeof(_mreport));
  memset(&_ureport,0,sizeof(_ureport));
}

//bluetooth
void Tab5BLECore::begin(const char* name,const char* manufacturer,bool bond){
  if(_begun){return;}
  _instance=this;
  BLEDevice::init(name);
  _server=BLEDevice::createServer();
  _server->setCallbacks(new ServerCallbackscore());
  _hidDevice=new BLEHIDDevice(_server);
  if(_gene){
    _inputKeyboard=_hidDevice->inputReport(1);_inputKeyboard->addDescriptor(new BLE2902());_inputKeyboard->setCallbacks(new KeyboardCallbacks());
    _inputMouse=_hidDevice->inputReport(2);_inputMouse->addDescriptor(new BLE2902());_inputMouse->setCallbacks(new MouseCallbacks());
  }
  else{
    _inputMouse=_hidDevice->inputReport(2);_inputMouse->addDescriptor(new BLE2902());_inputMouse->setCallbacks(new MouseCallbacks());
    _inputKeyboard=_hidDevice->inputReport(1);_inputKeyboard->addDescriptor(new BLE2902());_inputKeyboard->setCallbacks(new KeyboardCallbacks());
  }
  
  _hidDevice->manufacturer()->setValue(manufacturer);
  _hidDevice->pnp(0x02,_vid,_pid,_ver);
  _hidDevice->hidInfo(0x00,0x01);
  if(_gene){
    _hidDevice->reportMap((uint8_t*)REPORT_MAP_COMBO_TEST,sizeof(REPORT_MAP_COMBO_TEST));
  }
  else{
    _hidDevice->reportMap((uint8_t*)REPORT_MAP_COMBO_TEST2,sizeof(REPORT_MAP_COMBO_TEST2));
  }
  _hidDevice->startServices();
  _hidDevice->setBatteryLevel(100);
  if(bond){BLESecurity* sec=new BLESecurity();sec->setAuthenticationMode(ESP_LE_AUTH_BOND);}
  _advertising=BLEDevice::getAdvertising();
  _advertising->setAppearance(GENERIC_HID);
  _advertising->setScanResponse(true);
  _advertising->addServiceUUID(_hidDevice->hidService()->getUUID());
  _advertising->start();
  _begun=true;
}
void Tab5BLECore::disableBluetooth(){
  if(!_begun) return;
  if(_advertising) _advertising->stop();
  BLEDevice::deinit(true);
  _connected=false;
  _begun=false;
}
void Tab5BLECore::disconnectdevice(){
  if(!_connected||!_server)return;
  _server->disconnect(_connId);
  _connected=false;
  _connId = 0xFFFF;
  if(_advertising){_advertising->start();}
}
void Tab5BLECore::setBattery(uint8_t percent){_hidDevice->setBatteryLevel(percent);}
bool Tab5BLECore::isConnected() const{return _connected;}
bool Tab5BLECore::isBegin() const {return _begun;}
void Tab5BLECore::handleConnect(){_connected=true;}
void Tab5BLECore::handleDisconnect(){_connected=false;releaseAll();if(_advertising){_advertising->start();}}

//logic
bool Tab5BLECore::kbmod(uint8_t key) {return key==MOD_LCTRL||key==MOD_LSHIFT||key==MOD_LALT||key==MOD_RALT;}
bool Tab5BLECore::ifcondms() const{
  if(!_connected){Serial.println("Not connected");return true;}
  if(!_inputMouse){Serial.println("Mouse NULL");return true;}
  return false;
}
bool Tab5BLECore::ifcondkb() const{
  if(!_connected){Serial.println("Not connected");return true;}
  if(!_inputKeyboard){Serial.println("KEYBOARD NULL");return true;}
  return false;
}
bool Tab5BLECore::checkms() const{
  BLE2902* desc1 = (BLE2902*)_inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  if(desc1 && desc1->getNotifications()){return true;}
  return false;
}
bool Tab5BLECore::checkkb() const{
  BLE2902* desc2 = (BLE2902*)_inputKeyboard->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  if(desc2 && desc2->getNotifications()){return true;}
  return false;
}

//mode
void Tab5BLECore::setgene(bool mode){_gene=mode;}
bool Tab5BLECore::getgene() const{return _gene;}
void Tab5BLECore::setunified(bool mode){_unified=mode;}
bool Tab5BLECore::getunified() const{return _unified;}

//key
bool Tab5BLECore::releaseAll(){
  if(_gene){
    memset(&_kreport,0,sizeof(_kreport));sendReportkb();
    memset(&_mreport,0,sizeof(_mreport));sendReportms();
  }
  if(_unified){
    memset(&_ureport,0,sizeof(_ureport));sendReportuni();
  }
  return true;
}
bool Tab5BLECore::removeKey(uint8_t key){
  for(int i=0;i<6;i++){
    if(_gene){if(_kreport.keys[i]==key){_kreport.keys[i]=0;}}
    if(_unified){if(_ureport.keys[i]==key){_ureport.keys[i]=0;}}
  }
  return true;
}
bool Tab5BLECore::containsKey(uint8_t key){
  for(int i=0;i<6;i++){
    if(_gene){if(_kreport.keys[i]==key){return true;}}
    if(_unified){if(_ureport.keys[i]==key){return true;}}
  }
  return false;
}
bool Tab5BLECore::addKey(uint8_t key){
  if(key==0){return false;}
  if(containsKey(key)){return true;}
  for(int i=0;i<6;i++){
    if(_gene){if(_kreport.keys[i]==0){_kreport.keys[i]=key;return true;}}
    if(_unified){if(_ureport.keys[i]==0){_ureport.keys[i]=key;return true;}}
  }
  return false;
}

//report
bool Tab5BLECore::sendReportkb(){
  if(ifcondkb()){return false;}
  _inputKeyboard->setValue((uint8_t*)&_kreport,sizeof(_kreport));
  _inputKeyboard->notify();
  return true;
}
bool Tab5BLECore::sendReportms(){
  if(ifcondms()){return false;}
  _inputMouse->setValue((uint8_t*)&_mreport,sizeof(_mreport));
  //printf("[BLE MOUSE] size=%d | ",sizeof(_mreport));
  //if(_mreport.buttons==0x00){printf("[BUTTONS ZERO] | ");}else{printf("buttons=0x%02X | ",_mreport.buttons);}
  //if(_mreport.x==0&&_mreport.y==0){printf("[PAD ZERO]\n");}else{printf("x=%d | y=%d | ",_mreport.x,_mreport.y);}
  //String val=_inputMouse->getValue();
  //printf("[BLE CHAR RAW] ");
  //for(size_t i=0;i<val.length();i++){printf("%02X ",(uint8_t)val[i]);}
  //printf("\n");
  if(checkms()){_inputMouse->notify();}
  //else{printf("[SKIP NOTIFY] not subscribed\n");}
  _mreport.buttons = 0x00;
  _mreport.x = 0;
  _mreport.y = 0;
  _mreport.wheel = 0;
  return true;
}
bool Tab5BLECore::sendReportuni(){
  if(!_connected){return false;}
  _inputGeneric->setValue((uint8_t*)&_ureport,sizeof(_ureport));
  _inputGeneric->notify();
  return true;
}

//keyboard
bool Tab5BLECore::pressKey(uint8_t key){
  addKey(key);
  if(_gene){return sendReportkb();}
  if(_unified){return sendReportuni();}
  return false;
}
bool Tab5BLECore::releaseKey(uint8_t key){
  removeKey(key);
  if(_gene){return sendReportkb();}
  if(_unified){return sendReportuni();}
  return false;
}
bool Tab5BLECore::pressModifier(uint8_t mod){
  if(_gene){
    _kreport.modifiers|=mod;
    return sendReportkb();
  }
  if(_unified){
    _ureport.modifiers|=mod;
    return sendReportuni();
  }
  return false;
}
bool Tab5BLECore::releaseModifier(uint8_t mod){
  if(_gene){
    _kreport.modifiers&=~mod;
    return sendReportkb();
  }
  if(_unified){
    _ureport.modifiers&=~mod;
    return sendReportuni();
  }
  return false;
}
bool Tab5BLECore::release(uint8_t key){
  if(kbmod(key)){return releaseModifier(key);}
  return releaseKey(key);
}
bool Tab5BLECore::press(uint8_t key){
  if(kbmod(key)){return pressModifier(key);}
  return pressKey(key);
}
bool Tab5BLECore::tap(uint8_t key,uint16_t hold){press(key);delay(hold);release(key);return true;}
bool Tab5BLECore::write(uint8_t key){tap(key);return true;}
bool Tab5BLECore::asciiToHid(char c,uint8_t& keycode,uint8_t& modifierMask){
  keycode=0;modifierMask=0;
  if(c>='a'&&c<='z'){keycode= 0x04 + (c-'a');return true;}
  if(c>='A'&&c<='Z'){keycode= 0x04 + (c-'A');modifierMask=MOD_RSHIFT;return true;}
  if(c>='1'&&c<='9'){keycode= 0x1E + (c-'1');return true;}
  if(c=='0'){keycode=0x27;return true;}
  int lens=36,lens2=21,lens3=1,lens4=1,lens5=1,lenpass=0;
  char codes[]={'\n','\r','\r','\t',' ','-','_','=','+','[','{',']','}','\\','|',';',':','\'','"','~',',','<','.','>','/','?','!','@','#','$','%','^','&','*','(',')'};
  uint8_t kcodes[]={0x28,0x28,0x2A,0x2B,0x2C,0x2D,0x2D,0x2E,0x2E,0x2F,0x2F,0x30,0x30,0x31,0x31,0x33,0x33,0x34,0x34,0x35,0x36,0x36,0x37,0x37,0x38,0x38,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27};
  int rshi1[]={6,8,10,12,14,16,18,19,20,22,24,25,26,27,28,29,30,31,32,33,34},rshi2[]={100},rshi3[]={100},rshi4[]={100};
  for(int i=0;i<lens;i++){if(c==codes[i]){
    keycode=kcodes[i];
    int pos=0;
    for(int j=0;j<lens2;j++){if(c==rshi1[j]){pos=1;}}
    for(int j=0;j<lens3;j++){if(c==rshi2[j]){pos=2;}}
    for(int j=0;j<lens4;j++){if(c==rshi3[j]){pos=3;}}
    for(int j=0;j<lens5;j++){if(c==rshi4[j]){pos=4;}}
    switch (pos){
      case 0:modifierMask=MOD_NONE;break;
      case 1:modifierMask=MOD_RSHIFT;break;
      case 2:modifierMask=MOD_LSHIFT;break;
      case 3:modifierMask=MOD_RALT;break;
      case 4:modifierMask=MOD_LALT;break;
    }
    lenpass=1;
  }}
  if(lenpass==1){return true;}else{return false;}
}
bool Tab5BLECore::writeAscii(char c,uint16_t hold){
  uint8_t key=0,mod=0;
  if(!asciiToHid(c,key,mod)){return false;}
  pressModifier(mod);
  tap(key,hold);
  releaseModifier(mod);
  return true;
}

//mouse
void Tab5BLECore::move(int8_t x,int8_t y,int8_t wheel){
  if(ifcondms()){return;}
  if(_gene){
    _mreport.x=x;_mreport.y=y;_mreport.wheel=wheel;
    sendReportms();
  }
  if(_unified){
    _ureport.x=x;_ureport.y=y;_ureport.wheel=wheel;
    sendReportuni();
  }
}
void Tab5BLECore::pressButton(uint8_t button){
  if(ifcondms()){return;}
  if(_gene){
    _mreport.buttons|=button;
    sendReportms();
  }
  if(_unified){
    _ureport.buttons|=button;
    sendReportuni();
  }
}
void Tab5BLECore::releaseButton(uint8_t button){
  if(ifcondms()){return;}
  if(_gene){
    _mreport.buttons&=~button;
    sendReportms();
  }
  if(_unified){
    _ureport.buttons&=~button;
    sendReportuni();
  }
}
void Tab5BLECore::click(uint8_t button,uint16_t holdMs){pressButton(button);delay(holdMs);releaseButton(button);}

