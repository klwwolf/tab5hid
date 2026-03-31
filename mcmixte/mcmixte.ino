#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
#include "USB.h"
#include "Tab5USBHID.h"
#include "Tab5BLEcore.h"
#include "keyreg.h"
#include "keyreg2.h"

m5::touch_detail_t touchDetail;
LGFX_Button bts[7];
LGFX_Button btkk[60];
CUSTOMUSBHIDMouse Mouse;
CUSTOMUSBHIDKeyboard Keyboard;
Tab5BLECore blecore;
keyreg lreg;
keyreg2 lreg2;

bool 
  btEnabled=false,btEnabled2=false,
  btConnected=false,btConnected2=false,
  lockBT=false,lockBT2=false,
  lockR=false,lockL=false,lockSS=false,lockKBT=false,majlock=false,lockDBG=false,debugstt=false,asstt=false;
bool lockKey[60]={false},onToutch[8]={false},keyTouched[49]={false};
unsigned long padLastX=-1,padLastY=-1,X=900,DX=400,DY=30,sensy=1,kbtp=1,autosleep=0,asmax=60,timer=0,timerms=0;
int padX=0,padY=0,lasttoutch=0,mdbt=1;
const int sizea=20;
String txt[sizea];
static const char mapAzertyLower[26]={'q','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','a','r','s','t','u','v','z','x','y','w'};
static const char mapAzertyUpper[26]={'Q','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','A','R','S','T','U','V','Z','X','Y','W'};
static const char num1[10]={'0','1','2','3','4','5','6','7','8','9'};

void psv(int A,int B,int C,int D,String* txt,int idx,const char* fmt,...){
  char buf[96];
  va_list ap;va_start(ap,fmt);vsnprintf(buf,sizeof(buf),fmt,ap);va_end(ap);txt[idx]=buf;
  int difa=A+(C/2),difb=B+(D/2);
  M5.Lcd.drawRect(A,B,C,D,BLACK);
  M5.Display.drawString(txt[idx],difa,difb);
}
void button(int A,int B,int C,int D,String E,int F){
  int difa=A+(C/2),difb=B+(D/2),SX=2,SY=2;
  if(F==1){bts[0].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"L CLICK",SX,SY);}
  if(F==2){bts[1].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"R CLICK",SX,SY);}
  if(F==3){bts[2].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"BT",SX,SY);}
  if(F==4){bts[3].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"DBG VIEW",SX,SY);}
  if(F==5){bts[4].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"spd cursor",SX,SY);}
  if(F==6){bts[5].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"KBTYPE",SX,SY);}
  if(F==7){bts[6].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"BT Mode",SX,SY);}
  bts[F-1].drawButton();
}
void buttonkb(int A,int B,int C,int D,String E,int F){
  int difa=A+(C/2),difb=B+(D/2),SX=2,SY=2;
  if(F==1){btkk[0].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"A",SX,SY);}
  if(F==2){btkk[1].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"B",SX,SY);}
  if(F==3){btkk[2].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"C",SX,SY);}
  if(F==4){btkk[3].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"D",SX,SY);}
  if(F==5){btkk[4].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"E",SX,SY);}
  if(F==6){btkk[5].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"F",SX,SY);}
  if(F==7){btkk[6].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"G",SX,SY);}
  if(F==8){btkk[7].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"H",SX,SY);}
  if(F==9){btkk[8].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"I",SX,SY);}
  if(F==10){btkk[9].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"J",SX,SY);}
  if(F==11){btkk[10].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"K",SX,SY);}
  if(F==12){btkk[11].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"L",SX,SY);}
  if(F==13){btkk[12].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"M",SX,SY);}
  if(F==14){btkk[13].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"N",SX,SY);}
  if(F==15){btkk[14].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"O",SX,SY);}
  if(F==16){btkk[15].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"P",SX,SY);}
  if(F==17){btkk[16].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"Q",SX,SY);}
  if(F==18){btkk[17].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"R",SX,SY);}
  if(F==19){btkk[18].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"S",SX,SY);}
  if(F==20){btkk[19].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"T",SX,SY);}
  if(F==21){btkk[20].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"U",SX,SY);}
  if(F==22){btkk[21].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"V",SX,SY);}
  if(F==23){btkk[22].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"W",SX,SY);}
  if(F==24){btkk[23].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"X",SX,SY);}
  if(F==25){btkk[24].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"Y",SX,SY);}
  if(F==26){btkk[25].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"Z",SX,SY);}

  if(F==27){btkk[26].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"à 0",SX,SY);}
  if(F==28){btkk[27].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"& 1",SX,SY);}
  if(F==29){btkk[28].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"é 2",SX,SY);}
  if(F==30){btkk[29].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"\" 3",SX,SY);}
  if(F==31){btkk[30].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"' 4",SX,SY);}
  if(F==32){btkk[31].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"( 5",SX,SY);}
  if(F==33){btkk[32].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"- 6",SX,SY);}
  if(F==34){btkk[33].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"è 7",SX,SY);}
  if(F==35){btkk[34].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"_ 8",SX,SY);}
  if(F==36){btkk[35].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"ç 9",SX,SY);}

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

void initscreen(){
  M5.Lcd.fillScreen(WHITE);M5.Display.setRotation(3);M5.Display.setTextSize(2);M5.Display.setTextColor(BLACK);M5.Display.setTextDatum(middle_center);
  for(int i=0;i<7;i++){button(10+(170*i),600,170,100,"",i+1);}
  M5.Lcd.drawRect(900,140,400,450,BLACK);
  int 
    XKB=5,
    DXKB=74,DXKB2=98,
    maxX=3,
    YKB=5,
    DYKB=85,DYKB2=84,
    maxY=12,
    indexkb=1
  ;
  M5.Lcd.drawRect(XKB,YKB,890,590,BLACK);
  for(int ia=0;ia<maxX;ia++){for(int ib=0;ib<maxY;ib++){buttonkb(XKB+(DXKB*ib),YKB+(DYKB-10)+(DYKB*ia),DXKB,DYKB,"",indexkb);indexkb++;}}
  buttonkb(XKB+(DXKB2*1),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//supr
  buttonkb(XKB+(DXKB2*0),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//del
  buttonkb(XKB+(DXKB2*2),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//enter
  buttonkb(XKB+(DXKB2*0),YKB+(DYKB2*6),(DXKB2*2),DYKB2,"",indexkb);indexkb++;//space
  buttonkb(XKB+(DXKB2*4),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//tab
  buttonkb(XKB+(DXKB2*7),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//ctrl
  buttonkb(XKB+(DXKB2*8),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//alt
  buttonkb(XKB+(DXKB2*8),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//altgr
  buttonkb(XKB+(DXKB2*7),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//maj
  buttonkb(XKB+(DXKB2*3),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//up
  buttonkb(XKB+(DXKB2*3),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//down
  buttonkb(XKB+(DXKB2*2),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//left
  buttonkb(XKB+(DXKB2*4),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//right
}

void setup(){
  Serial.begin(115200);
  auto cfg=M5.config();M5.begin(cfg);
  M5.Imu.begin();
  USB.begin();Mouse.begin();Keyboard.begin();
  initscreen();
  for(int i=0;i<sizea;i++){txt[i]="";}
  blecore.setgene(true);
  blecore.setunified(false);
}

void state(){
  /*
    * PS[]:
    * global     | X  | Y  |
    * fillrec    |    |    | DX      | DY 
    * psv        |    |    | DX1 DX2 | DY1
    * batdrawbar | X2 | Y2 | DX3     | DY3
    * batsegbar  |    |    | DX4     | DY4
  */
  int PS[]={900,5,400,120,400,200,25,30,1130,95,140,30,30,26};
  float ax,ay,az,gx,gy,gz,bat=M5.Power.getBatteryLevel();
  float lvmin[]={0,15,25,50,75},lvmax[]={15,25,50,75,101};
  int cons=M5.Power.getBatteryCurrent(),charA=-1,lvstt=-1;
  M5.Lcd.fillRect(PS[0],PS[1],PS[2],PS[3],WHITE);
  for(int i=0;i<5;i++){if(bat>=lvmin[i]&&bat<lvmax[i]){lvstt=i;}}
  if(cons<10){charA=1;}else{charA=0;}
  btConnected=blecore.isConnected();
  bool mma=false,mmb=false;
  if(btConnected){
    mma=blecore.checkkb();
    mmb=blecore.checkms();
  }
  M5.Imu.getAccel(&ax,&ay,&az);
  M5.Imu.getGyro(&gx,&gy,&gz);
  int Cmem=BLACK;
  if(btEnabled){blecore.setBattery(bat);}
  if(charA==0){Cmem=RED;autosleep=0;}else{if(cons>-5&&cons<20){Cmem=GREEN;autosleep+=1;}else{Cmem=ORANGE;autosleep=0;}}
  psv(PS[0],PS[1],PS[4],PS[6],txt,0,
    "BT:%d,%d|%d:%d,%d sp:%d|%d ts:%d",
    btEnabled,btConnected,mdbt,mma,mmb,
    sensy,kbtp,(asmax-autosleep)
  );
  psv(PS[0],PS[1]+30,PS[4],PS[6],txt,1,"ACC %.2f %.2f %.2f",ax,ay,az);
  psv(PS[0],PS[1]+60,PS[4],PS[6],txt,2,"GYR %.2f %.2f %.2f",gx,gy,gz);
  M5.Lcd.drawRect(PS[8],PS[9],PS[10],PS[11],Cmem);
  if(lvstt==0){M5.Lcd.fillRect(PS[8]+(35*0)+2,PS[9]+2,PS[12],PS[13],RED);}else{for(int i=0;i<lvstt;i++){M5.Lcd.fillRect(PS[8]+(35*i)+2,PS[9]+2,PS[12],PS[13],GREEN);}}
}
void debug(bool A[],bool B[],unsigned long C,unsigned long D,int E,int F,int G){
  int PS[]={ 900,5, 400,120, 400,30 };
  M5.Lcd.fillRect(PS[0],PS[1],PS[2],PS[3],WHITE);
  String dbtxt[1]={"null"};
  //debug(onToutch,keyTouched,padLastX,padLastY,padX,padY);
  psv(PS[0],PS[1],PS[4],PS[5],txt,0,"toutch:%d|%d,%d,%d,%d,%d,%d,%d,%d",G,A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7]);
  psv(PS[0],PS[1]+30,PS[4],PS[5],txt,0,"pad:%d,%d|%d,%d",C,E,D,F);
  psv(PS[0],PS[1]+60,PS[4],PS[5],txt,0,"kb: %d",majlock);
  psv(PS[0],PS[1]+90,PS[4],PS[5],txt,0,"null");
}

void bt_ctl(bool cmd){
  if(cmd){
    blecore.begin("Tab5BLEHID","M5Stack",true);
    btEnabled=blecore.isBegin();
  }else{
    if(btEnabled){
      if(blecore.isConnected()){blecore.disconnectdevice();}
      else{blecore.disableBluetooth();btEnabled=false;}
    }
  }
}
void dbg_ctl(bool cmd){debugstt=cmd;}



void keyAction(int k){
  char key1,key2;
  if(k<26){key1=majlock?mapAzertyUpper[k]:mapAzertyLower[k];}if(k>=26){key1=num1[k-26];}
  if(kbtp==1){if(k<26){key2=majlock?mapAzertyUpper[k]:mapAzertyLower[k];}if(k>=26){key2=num1[k-26];}}
  if(kbtp==2){if(k<26){key2=majlock?('A'+k):('a'+k);}if(k>=26){key2='0'+(k-26);}}
  if(kbtp==3){if(k<26){key2=majlock?(lreg.A+k):(lreg.a+k);}if(k>=26){key2=lreg.NUM_0+(k-26);}}
  if(kbtp==4){if(k<26){key2=majlock?(lreg2.A+k):(lreg2.a+k);}if(k>=26){key2=lreg2.NUM_0+(k-26);}}
  bool exec=btEnabled&&blecore.isConnected();
  if(exec){blecore.writeAscii(key2);}else{Keyboard.write(key1);}
}
void keyAct(int k,bool mds){
  uint8_t code1=0x00,code2=0x00;
  switch(k){
    case 36:code1=lreg2.DELETE;code2=lreg.DELETE;break;
    case 37:code1=lreg2.BACKSPACE;code2=lreg.BACKSPACE;break;
    case 38:code1=lreg2.ENTER;code2=lreg.ENTER;break;
    case 39:code1=lreg2.SPACE;code2=lreg.SPACE;break;
    case 40:code1=lreg2.TAB;code2=lreg.TAB;break;
    case 41:code1=lreg2.LCTRL;code2=lreg.LCTRL;break;
    case 42:code1=lreg2.LALT;code2=lreg.LALT;break;
    case 43:code1=lreg2.RALT;code2=lreg.RALT;break;
    case 44:code1=lreg2.LSHIFT;code2=lreg.LSHIFT;majlock=mds;break;
    case 45:code1=lreg2.UP;code2=lreg.UP;break;
    case 46:code1=lreg2.DOWN;code2=lreg.DOWN;break;
    case 47:code1=lreg2.LEFT;code2=lreg.LEFT;break;
    case 48:code1=lreg2.RIGHT;code2=lreg.RIGHT;break;
  }
  if(code1!=0x00){
    bool exec=btEnabled&&blecore.isConnected();
    if(exec){if(mds){blecore.press(code2);}else{blecore.release(code2);}}
    else{if(mds){Keyboard.press(code1);}else{Keyboard.release(code1);}}
  }
}

void toutch(){
  int touches=M5.Touch.getCount();
  if(touches==0){
    padLastX=-1;padLastY=-1;padX=0;padY=0;
    if(lockR){Mouse.release(MOUSE_RIGHT);lockR=false;}
    if(lockL){Mouse.release(MOUSE_LEFT);lockL=false;}
    if(lockSS){lockSS=false;}
    if(lockKBT){lockKBT=false;}
    if(lockDBG){lockDBG=false;}
    if(lockBT){lockBT=false;}
    if(lockBT2){lockBT2=false;}
    for(int k=36;k<49;k++){
      if(lockKey[k]){
        keyAct(k,false);
        lockKey[k]=false;
      }
    }
  }

  for(int k=0;k<7;k++){onToutch[k]=false;}
  for(int k=0;k<49;k++){keyTouched[k]=false;}

  if(touches>0)for(int i=0;i<touches;i++){
    autosleep=0;
    auto t=M5.Touch.getDetail(i);
    int x=t.x,y=t.y;
    for(int k=0;k<49;k++){if(btkk[k].contains(x,y)){keyTouched[k]=true;}else{keyTouched[k]=false;}}
    for(int k=0;k<7;k++){if(bts[k].contains(x,y)){onToutch[k]=true;}else{onToutch[k]=false;}}
    if(x>900&&x<1300&&y>140&&y<590){onToutch[7]=true;padX=x;padY=y;}
  }

  for(int k=0;k<49;k++){
    if(keyTouched[k]){
      if(!lockKey[k]){
        if(k<36){keyAction(k);}
        else{keyAct(k,true);}
        lockKey[k]=true;
      }
    }
    else{
      if(k<36){lockKey[k]=false;}
      else{
        if(lockKey[k]){
          keyAct(k,false);
          lockKey[k]=false;
        }
      }
    }
  }


  bool exec=btEnabled&&blecore.isConnected();
  if(onToutch[0]){if(!lockL){if(exec){blecore.pressButton(blecore.M_LEFT);}else{Mouse.press(MOUSE_LEFT);}lockL=true;}}
  if(!onToutch[0]){if(lockL){if(exec){blecore.releaseButton(blecore.M_LEFT);}else{Mouse.release(MOUSE_LEFT);}lockL=false;}}
  if(onToutch[1]){if(!lockR){if(exec){blecore.pressButton(blecore.M_RIGHT);}else{Mouse.press(MOUSE_RIGHT);}lockR=true;}}
  if(!onToutch[1]){if(lockR){if(exec){blecore.releaseButton(blecore.M_RIGHT);}else{Mouse.release(MOUSE_RIGHT);}lockR=false;}}
  if(onToutch[2]){if(!lockBT){if(btEnabled){bt_ctl(false);}else{bt_ctl(true);}lockBT=true;}}
  if(onToutch[3]){if(!lockDBG){if(debugstt){dbg_ctl(false);}else{dbg_ctl(true);}lockDBG=true;}}
  if(onToutch[4]){if(!lockSS){if(sensy==1){sensy=2;}else if(sensy==2){sensy=3;}else{sensy=1;}lockSS=true;}}
  if(onToutch[5]){if(!lockKBT){if(kbtp==1){kbtp=2;}else{if(kbtp==2){kbtp=3;}else{if(kbtp==3){kbtp=4;}else{kbtp=1;}}}lockKBT=true;}}
  if(onToutch[6]){if(!lockBT2){if(mdbt==1){mdbt=2;blecore.setgene(false);blecore.setunified(true);}else{mdbt=1;blecore.setgene(true);blecore.setunified(false);}lockBT2=true;}}
  if(onToutch[7]){
    if(padLastX==-1){padLastX=padX;padLastY=padY;return;}
    int dx=padX-padLastX;int dy=padY-padLastY;padLastX=padX;padLastY=padY;
    bool blockpass = dx==0 && dy==0;
    if(!blockpass){
      if(exec){unsigned long nowms=millis();if(nowms-timerms>=10){timerms=nowms;blecore.move(dx*sensy,dy*sensy);}}
      else{Mouse.move(dx*sensy,dy*sensy);}
    }
  }else{padLastX=-1;padLastY=-1;}

  if(debugstt&&lasttoutch!=0){debug(onToutch,keyTouched,padLastX,padLastY,padX,padY,touches);}
  if(touches!=lasttoutch){lasttoutch=touches;}
}

void loop(){
  M5.update();
  toutch();
  unsigned long now=millis();
  if(now-timer>=1000){timer=now;if(!debugstt){state();}}
  if((autosleep-asmax)<=0){M5.Power.powerOff();}
  delay(2);
}