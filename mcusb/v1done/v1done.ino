#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
#include "USB.h"
#include "Tab5USBHID.h"
#include "keyreg.h"
#include "keyreg2.h"

m5::touch_detail_t touchDetail;
LGFX_Button bts[7];
LGFX_Button btkk[60];
CUSTOMUSBHIDMouse Mouse;
CUSTOMUSBHIDKeyboard Keyboard;
keyreg lreg;
keyreg2 lreg2;

bool 
  lockR=false,lockL=false,lockSS=false,lockKBT=false,majlock=false,lockDBG=false,debugstt=false;
bool lockKey[60]={false},onToutch[8]={false},keyTouched[49]={false};
unsigned long padLastX=-1,padLastY=-1,X=900,DX=400,DY=30,sensy=1,kbtp=1,autosleep=0,asmax=60,timer=0;
int padX=0,padY=0,lasttoutch=0;
const int sizea=20;
String txt[sizea];
const char* AZERTYUP="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* AZERTYDOWN="abcdefghijklmnopqrstuvwxyz";
const char* NUMBER="0123456789";
static const char mapAzertyLower[26]={'q','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','a','r','s','t','u','v','z','x','y','w'};
static const char mapAzertyUpper[26]={'Q','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','A','R','S','T','U','V','Z','X','Y','W'};
static const char num1[10]={'0','1','2','3','4','5','6','7','8','9'};

//optimise function
void psv(int A,int B,int C,int D,String* txt,int idx,const char* fmt,...){
  char buf[96];
  va_list ap;va_start(ap,fmt);vsnprintf(buf,sizeof(buf),fmt,ap);va_end(ap);txt[idx]=buf;
  int difa=A+(C/2),difb=B+(D/2);
  M5.Lcd.drawRect(A,B,C,D,BLACK);
  M5.Display.drawString(txt[idx],difa,difb);
}
char getchars(int reg,int pos){
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
  return charlist[pos];
}
const char* charToStr(char c){static char buf[2];buf[0]=c;buf[1]='\0';return buf;}
void buttons(int select, int A,int B,int C,int D,String E,int F){
  if(select==0){
    int difa=A+(C/2),difb=B+(D/2),SX=2,SY=2;
    if(F==1){bts[0].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"L CLICK",SX,SY);}
    if(F==2){bts[1].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"R CLICK",SX,SY);}
    if(F==3){bts[2].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"spd cursor",SX,SY);}
    if(F==4){bts[3].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"DBG VIEW",SX,SY);}
    if(F==5){bts[4].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"KBTYPE",SX,SY);}
    if(F==6){bts[5].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"none",SX,SY);}
    if(F==7){bts[6].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,"none",SX,SY);}
    bts[F-1].drawButton();
  }
  if(select==1){
    int difa=A+(C/2),difb=B+(D/2),SX=2,SY=2;
    for(int i=0;i<26;i++){int selected=i+1;if(F==selected){
      const char* schar=charToStr(getchars(0,i));
      btkk[i].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,schar,SX,SY);
    }}
    for(int i=26;i<36;i++){int selected=i+1;if(F==selected){
      const char* schar=charToStr(getchars(2,i-26));
      btkk[i].initButton(&M5.Lcd,difa,difb,C,D,TFT_BLUE,TFT_YELLOW,TFT_BLACK,schar,SX,SY);
    }}
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

void initscreen(){
  M5.Lcd.fillScreen(WHITE);
  M5.Display.setRotation(3);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(BLACK);
  M5.Display.setTextDatum(middle_center);

  for(int i=0;i<7;i++){buttons(0,10+(170*i),600,170,100,"",i+1);}
  mousepad(0,0,0);

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
  for(int ia=0;ia<maxX;ia++){for(int ib=0;ib<maxY;ib++){buttons(1,XKB+(DXKB*ib),YKB+(DYKB-10)+(DYKB*ia),DXKB,DYKB,"",indexkb);indexkb++;}}
  buttons(1,XKB+(DXKB2*1),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//supr
  buttons(1,XKB+(DXKB2*0),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//del
  buttons(1,XKB+(DXKB2*2),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//enter
  buttons(1,XKB+(DXKB2*0),YKB+(DYKB2*6),(DXKB2*2),DYKB2,"",indexkb);indexkb++;//space
  buttons(1,XKB+(DXKB2*4),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//tab
  buttons(1,XKB+(DXKB2*7),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//ctrl
  buttons(1,XKB+(DXKB2*8),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//alt
  buttons(1,XKB+(DXKB2*8),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//altgr
  buttons(1,XKB+(DXKB2*7),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//maj
  buttons(1,XKB+(DXKB2*3),YKB+(DYKB2*5),DXKB2,DYKB2,"",indexkb);indexkb++;//up
  buttons(1,XKB+(DXKB2*3),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//down
  buttons(1,XKB+(DXKB2*2),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//left
  buttons(1,XKB+(DXKB2*4),YKB+(DYKB2*6),DXKB2,DYKB2,"",indexkb);indexkb++;//right
}

//interface data
void dbg_ctl(bool cmd){debugstt=cmd;}
void state(){
  int PS[]={900,5,/*0,0,   0,0,*/400,120,   /*0,0,*/400,25,   1130,65,140,30,   /*0,0,*/30,26};
  float ag[6],bat=M5.Power.getBatteryLevel(),lvstt=-1,lvmin[]={0,15,25,50,75},lvmax[]={15,25,50,75,101},camAh=0.0,rmAh=0.0,cmA=0.0,thour=0.0;
  int testC=0,Cmem=BLACK,colors=BLACK,nbbar=-1,charA=-1,cons=M5.Power.getBatteryCurrent(),volt=M5.Power.getBatteryVoltage(),mins=0,hours=0;

  M5.Imu.getAccel(&ag[0],&ag[1],&ag[2]);M5.Imu.getGyro(&ag[3],&ag[4],&ag[5]);
  camAh=2000.0;rmAh=camAh*(bat/100.0);cmA=abs(cons);thour=rmAh/cmA;mins=thour*60;hours=mins/60;mins=mins-(60*hours);
  for(int i=0;i<5;i++){if(bat>=lvmin[i]&&bat<lvmax[i]){lvstt=i;}}
  testC=((1)*((lvstt>1)?(lvstt-1):(0)));charA=(cons<10)?(1):(0);
  autosleep=(charA==1)?((cons>-5&&cons<20)?(autosleep+1):(0)):(0);
  Cmem=(charA==1)?((cons>-5&&cons<20)?(GREEN):(ORANGE)):(RED);
  colors=(lvstt==0)?(RED):(GREEN);nbbar=(1)+(testC);

  M5.Lcd.fillRect(PS[0],PS[1],PS[2],PS[3],WHITE);
  psv(PS[0],PS[1],PS[4],PS[5],txt,0,"sp:%d:%d | ts:%d | %d:%d",sensy,kbtp,(asmax-autosleep),hours,mins);
  psv(PS[0],PS[1]+30,PS[4],PS[5],txt,1,"%.1f:%.1f:%.1f %.1f:%.1f:%.1f",ag[0],ag[1],ag[2],ag[3],ag[4],ag[5]);
  //psv(PS[0],PS[1]+60,PS[4],PS[5],txt,2,"");
  //psv(PS[0],PS[1]+90,PS[4],PS[5],txt,3,"");
  M5.Lcd.drawRect(PS[6],PS[7],PS[8],PS[9],Cmem);for(int i=0;i<nbbar;i++){M5.Lcd.fillRect(PS[6]+(35*i)+2,PS[7]+2,PS[10],PS[11],colors);}
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

//middle brain
void mousepad(int reg,int x,int y){
  int xms=900,yms=140,dxms=400,dyms=450;
  if(reg==0){M5.Lcd.drawRect(xms,yms,dxms,dxms,BLACK);}//mousepad(0,0,0);
  if(reg==1){bool ist=((x>xms)&&(x<(xms+dxms)))&&((y>yms)&&(y<(yms+dyms)));onToutch[7]=ist;if(ist){padX=x;padY=y;}else{padX=-1;padY=-1;}}//mousepad(1,x,y);
  if(reg==2){
    if(onToutch[7]){
      if(padLastX==-1){padLastX=padX;}int dx=padX-padLastX;padLastX=padX;
      if(padLastY==-1){padLastY=padY;}int dy=padY-padLastY;padLastY=padY;
      bool blockpass=((dx==0)&&(dy==0));
      if(!blockpass){Mouse.move(dx*sensy,dy*sensy);}
    }else{padLastX=-1;padLastY=-1;}
  }//mousepad(2,0,0);
  if(reg==3){padLastX=-1;padLastY=-1;padX=0;padY=0;onToutch[7]=false;}//mousepad(3,0,0);
}
void keys(int reg,int k,bool mds){
  if(reg==0){
    char key1,key2;
    if(k<26){key1=majlock?mapAzertyUpper[k]:mapAzertyLower[k];}if(k>=26){key1=num1[k-26];}
    if(kbtp==1){if(k<26){key2=majlock?mapAzertyUpper[k]:mapAzertyLower[k];}if(k>=26){key2=num1[k-26];}}
    if(kbtp==2){if(k<26){key2=majlock?('A'+k):('a'+k);}if(k>=26){key2='0'+(k-26);}}
    if(kbtp==3){if(k<26){key2=majlock?(lreg.A+k):(lreg.a+k);}if(k>=26){key2=lreg.NUM_0+(k-26);}}
    if(kbtp==4){if(k<26){key2=majlock?(lreg2.A+k):(lreg2.a+k);}if(k>=26){key2=lreg2.NUM_0+(k-26);}}
    Keyboard.write(key1);
  }
  if(reg==1){
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
      if(mds){Keyboard.press(code1);}else{Keyboard.release(code1);}
    }
  }
}

//touch center brain
void toutch(){
  int touches=M5.Touch.getCount();
  if(touches==0){
    mousepad(3,0,0);
    if(lockR){Mouse.release(MOUSE_RIGHT);lockR=false;}
    if(lockL){Mouse.release(MOUSE_LEFT);lockL=false;}
    if(lockSS){lockSS=false;}
    if(lockKBT){lockKBT=false;}
    if(lockDBG){lockDBG=false;}
    for(int k=36;k<49;k++){if(lockKey[k]){keys(1,k,false);lockKey[k]=false;}}
    for(int k=0;k<8;k++){onToutch[k]=false;}
    for(int k=0;k<49;k++){keyTouched[k]=false;}
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
    if(keyTouched[k]){
      if(!lockKey[k]){
        if(k<36){keys(0,k,false);}else{keys(1,k,true);}
        lockKey[k]=true;
      }
    }
    else{
      if(k<36){lockKey[k]=false;}
      else{if(lockKey[k]){keys(1,k,false);lockKey[k]=false;}}
    }
  }
  if(onToutch[0]){if(!lockL){Mouse.press(MOUSE_LEFT);lockL=true;}}else{if(lockL){Mouse.release(MOUSE_LEFT);lockL=false;}}
  if(onToutch[1]){if(!lockR){Mouse.press(MOUSE_RIGHT);lockR=true;}}else{if(lockR){Mouse.release(MOUSE_RIGHT);lockR=false;}}
  if(onToutch[2]){if(!lockSS){if(sensy==1){sensy=2;}else if(sensy==2){sensy=3;}else{sensy=1;}lockSS=true;}}else{if(lockSS){lockSS=false;}}
  if(onToutch[3]){if(!lockDBG){if(debugstt){dbg_ctl(false);}else{dbg_ctl(true);}lockDBG=true;}}else{if(lockDBG){lockDBG=false;}}
  if(onToutch[4]){if(!lockKBT){if(kbtp==1){kbtp=2;}else{if(kbtp==2){kbtp=3;}else{if(kbtp==3){kbtp=4;}else{kbtp=1;}}}lockKBT=true;}}else{if(lockKBT){lockKBT=false;}}
  if(onToutch[5]){}
  if(onToutch[6]){}
  mousepad(2,0,0);
  if(debugstt&&lasttoutch!=0){debug(onToutch,keyTouched,padLastX,padLastY,padX,padY,touches);}
  if(touches!=lasttoutch){lasttoutch=touches;}
}


//initial
void setup(){
  Serial.begin(115200);
  auto cfg=M5.config();M5.begin(cfg);
  M5.Imu.begin();
  USB.begin();
  Mouse.begin();Keyboard.begin();
  initscreen();
  for(int i=0;i<sizea;i++){txt[i]="";}
}
void loop(){
  M5.update();
  toutch();
  unsigned long now=millis();
  if(now-timer>=1000){timer=now;if(!debugstt){state();}}
  if((autosleep-asmax)<=0){M5.Power.powerOff();}
  delay(2);
}