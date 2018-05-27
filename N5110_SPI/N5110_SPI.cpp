#include "N5110_SPI.h"

#if USESPI==1
#include <SPI.h>
#endif

const uint8_t initdata[] PROGMEM = {
  PCD8544_FUNCTIONSET | PCD8544_EXTINSTRUCTION,
	PCD8544_SETVOP | DEFAULT_CONTRAST,
	PCD8544_SETTEMP | DEFAULT_TEMP,
	PCD8544_SETBIAS | DEFAULT_BIAS,
	PCD8544_FUNCTIONSET,
	PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL
};
// ---------------------------------
N5110_SPI::N5110_SPI(byte rst, byte cs, byte dc)
{
  rstPin = rst;
  csPin  = cs;
  dcPin  = dc;
}
// ---------------------------------
N5110_SPI::N5110_SPI(byte rst, byte cs, byte dc, byte din, byte clk)
{
  rstPin = rst;
  csPin  = cs;
  dcPin  = dc;
  clkPin = clk;
  dinPin = din;
  //ledPin = led;
}
// ---------------------------------
void N5110_SPI::init()
{
  cr = 0;
  font = NULL;
  dualChar = 0;
  pinMode(csPin, OUTPUT);
  pinMode(dcPin, OUTPUT);
  pinMode(dinPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  
  //pinMode(ledPin, OUTPUT);
  //analogWrite(ledPin, 200); //PWM Light optional, moved to application code

  if(rstPin<255) {
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, LOW);
    digitalWrite(rstPin, HIGH);
  }
  
#if USESPI==1
  SPI.begin();
#endif
  
  setCmd();
  startCS();
  sendDataF(initdata, sizeof(initdata));
  stopCS();
}
// ----------------------------------------------------------------
inline void N5110_SPI::sendData(byte data)
{
#if USESPI==1
  SPI.transfer(data);
#else
  shiftOut(dinPin, clkPin, MSBFIRST, data);
#endif
}
// ----------------------------------------------------------------
inline void N5110_SPI::sendData(const uint8_t *data, int cnt)
{
  for(int i=0; i<cnt; i++)
#if USESPI==1
    SPI.transfer(*data++);
#else
    shiftOut(dinPin, clkPin, MSBFIRST, *data++);
#endif
}
// ----------------------------------------------------------------
inline void N5110_SPI::sendDataF(const uint8_t *data, int cnt)
{
  for(int i=0; i<cnt; i++)
#if USESPI==1
    SPI.transfer(pgm_read_byte_near(data+i));
#else
    shiftOut(dinPin, clkPin, MSBFIRST, pgm_read_byte_near(data + i));
#endif
}
// ----------------------------------------------------------------
void N5110_SPI::gotoXY(byte x, byte y)
{
  setCmd();
  startCS();
  sendData(PCD8544_SETXADDR | x);
  sendData(PCD8544_SETYADDR | y);
  stopCS();
}
// ----------------------------------------------------------------
void N5110_SPI::sleep(bool mode)
{
  if(mode) {
    clrScr();
    setCmd();
    startCS();
	  sendData(PCD8544_FUNCTIONSET | PCD8544_POWERDOWN);
    stopCS();
  } else {
    setCmd();
    startCS();
    sendData(PCD8544_FUNCTIONSET | PCD8544_EXTINSTRUCTION);
	  sendData(PCD8544_SETVOP | DEFAULT_CONTRAST); // contrast
	  sendData(PCD8544_SETTEMP | DEFAULT_TEMP);
	  sendData(PCD8544_SETBIAS | DEFAULT_BIAS);
	  sendData(PCD8544_FUNCTIONSET);
	  sendData(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);
    stopCS();
  }
}
// ----------------------------------------------------------------
void N5110_SPI::setContrast(byte val)
{
  if(val>0x7f) val=0x7f;
  if(val<0) val=0;
  setCmd();
  startCS();
	sendData(PCD8544_FUNCTIONSET | PCD8544_EXTINSTRUCTION);
	sendData(PCD8544_SETVOP | val);
  sendData(PCD8544_FUNCTIONSET);
  stopCS();
}
// ----------------------------------------------------------------
void N5110_SPI::setBias(byte val)
{
  if(val>7) val=7;
  if(val<0) val=0;
  setCmd();
  startCS();
	sendData(PCD8544_FUNCTIONSET | PCD8544_EXTINSTRUCTION);
	sendData(PCD8544_SETBIAS | val);
  sendData(PCD8544_FUNCTIONSET);
  stopCS();
}
// ----------------------------------------------------------------
void N5110_SPI::setTemp(byte val)
{
  if(val>3) val=3;
  if(val<0) val=0;
  setCmd();
  startCS();
	sendData(PCD8544_FUNCTIONSET | PCD8544_EXTINSTRUCTION);
  sendData(PCD8544_SETTEMP | val);
  sendData(PCD8544_FUNCTIONSET);
  stopCS();
}
// ----------------------------------------------------------------
// val=PCD8544_DISPLAYBLANK, PCD8544_DISPLAYNORMAL, PCD8544_DISPLAYALLON, PCD8544_DISPLAYINVERTED
void N5110_SPI::displayMode(byte val)
{
  setCmd();
  startCS();
	sendData(PCD8544_DISPLAYCONTROL | val);
  stopCS();
}
// ----------------------------------------------------------------
void N5110_SPI::clrScr()
{
  gotoXY(0,0);
  setDat();
  startCS();
  for (int i=0; i<SCR_WD*SCR_HT/8/4; i++)
  {
#if USESPI==1
    SPI.transfer(0);
    SPI.transfer(0);
    SPI.transfer(0);
    SPI.transfer(0);
#else
    shiftOut(dinPin, clkPin, MSBFIRST, 0);
    shiftOut(dinPin, clkPin, MSBFIRST, 0);
    shiftOut(dinPin, clkPin, MSBFIRST, 0);
    shiftOut(dinPin, clkPin, MSBFIRST, 0);
#endif
  }
  stopCS();
  gotoXY(0, 0);
}
// ---------------------------------
#define ALIGNMENT \
  if(x==-1)\
    x = SCR_WD-wd; \
  else if(x<0) \
    x = (SCR_WD-wd)/2; \
  if(x<0) x=0; \
  if(x>=SCR_WD || y8>=SCR_HT/8) return 0; \
  if(x+wd>SCR_WD) wd = SCR_WD-x; \
  if(y8+ht8>SCR_HT/8) ht8 = SCR_HT/8-y8

int N5110_SPI::fillWin(int x, uint8_t y8, uint8_t wd, uint8_t ht8, uint8_t data)
{
  ALIGNMENT;
  for(int i=0; i<ht8; i++) {
    gotoXY(x,y8+i);
    setDat();
	  startCS();
	  for(int j=0; j<wd; j++) sendData(data);
    stopCS();
  }
  return x+wd;
}
// ---------------------------------
int N5110_SPI::drawBuf(const uint8_t *bmp, int x, uint8_t y8, uint8_t wd, uint8_t ht8)
{
  uint8_t wdb = wd;
  ALIGNMENT;
  for(int i=0; i<ht8; i++) {
    gotoXY(x,y8+i);
    setDat();
  	startCS();
    sendData(bmp+wdb*i, wd);
    stopCS();
  }
  return x+wd;
}
// ---------------------------------
int N5110_SPI::drawBitmap(const uint8_t *bmp, int x, uint8_t y8, uint8_t wd, uint8_t ht8)
{
  uint8_t wdb = wd;
  ALIGNMENT;
  for(int i=0; i<ht8; i++) {
    gotoXY(x,y8+i);
    setDat();
  	startCS();
    sendDataF(bmp+wdb*i, wd);
    stopCS();
  }
  return x+wd;
}
// ---------------------------------
int N5110_SPI::drawBitmap(const uint8_t *bmp, int x, uint8_t y8)
{
  uint8_t wd = pgm_read_byte(bmp+0);
  uint8_t ht = pgm_read_byte(bmp+1);
  return drawBitmap(bmp+2, x, y8, wd, ht);
}
// ---------------------------------
// text rendering stuff
// ---------------------------------
void N5110_SPI::setFont(const uint8_t* f)
{
  font     = f;
  xSize    =-pgm_read_byte(font+0);
  ySize    = pgm_read_byte(font+1);
  firstCh  = pgm_read_byte(font+2);
  lastCh   = pgm_read_byte(font+3);
  ySize8   = (ySize+7)/8;
  minCharWd  = 0;
  minDigitWd = 0;
}
// ---------------------------------
int N5110_SPI::printStr(int x, uint8_t y8, char *txt)
{
  if(!font) return 0;
  int xpos = x;
  int ypos = y8;
  if(xpos==-1)
    xpos = SCR_WD-strWidth(txt);  // align to right
  else if(xpos<0)
    xpos = (SCR_WD-strWidth(txt))/2;  // center
  if(xpos<0) xpos=0;
  while (*txt) {
    if(*txt==10) { // \n clears area to the end of line and jumps to the next line
      fillWin(xpos,ypos,SCR_WD-xpos,ySize8,0);
      xpos = 0;
      ypos += ySize8;
      txt++;
    } else {
      int wd = printChar(xpos, ypos, *txt++);
      xpos += wd;
      if(cr && xpos>=SCR_WD) {
        xpos = 0;
        ypos += ySize8;
        if(ypos>SCR_HT/8)
          ypos = 0;
      }
    }
  }
  return xpos;
}
// ---------------------------------
int N5110_SPI::strWidth(char *txt)
{
  if(!font || !txt || !*txt) return 0;
  int wd = 0;
  char ch;
  while(*txt)
    if(*txt<' ') txt++; else wd+=charWidth((ch=*txt++));
  return wd-(charWidth(ch,true)-charWidth(ch,false));
}
// ---------------------------------
int N5110_SPI::charWidth(uint8_t _ch, bool last)
{
  int ch = convertPolish(_ch);
  if(!font || ch<firstCh || ch>lastCh) return 0;
  int idx = 4 + (ch - firstCh)*(xSize*ySize8+1);
  int wd = pgm_read_byte(font + idx);
  int wdL = 0, wdR = 1; // default spacing before and behind char
  if(isdigit(ch)) {
    if(minDigitWd>wd) {
      wdL = (minDigitWd-wd)/2;
      wdR += (minDigitWd-wd-wdL);
    }
  } else
  if(minCharWd>wd) {
    wdL = (minCharWd-wd)/2;
    wdR += (minCharWd-wd-wdL);
  }
  return last ? wd+wdL+wdR : wd+wdL;  // last!=0 -> get rid of last empty columns
}
// ---------------------------------
int N5110_SPI::printChar(int x, uint8_t row, uint8_t _ch)
{
  int ch = convertPolish(_ch);
  if(!font || ch<firstCh || ch>lastCh || x>=SCR_WD || row>=SCR_HT/8) return 0;

  int j,i, idx = 4 + (ch - firstCh)*(xSize*ySize8+1);
  int wd = pgm_read_byte(font + idx++);
  int wdL = 0, wdR = 1; // default spacing before and behind char
  if(isdigit(ch)) {
    if(minDigitWd>wd) {
      wdL = (minDigitWd-wd)/2;
      wdR += (minDigitWd-wd-wdL);
    }
  } else
  if(minCharWd>wd) {
    wdL = (minCharWd-wd)/2;
    wdR += (minCharWd-wd-wdL);
  }
  if(x+wd+wdL+wdR>SCR_WD) wdR = max(SCR_WD-x-wdL-wd, 0);
  if(x+wd+wdL+wdR>SCR_WD) wd = max(SCR_WD-x-wdL, 0);
  if(x+wd+wdL+wdR>SCR_WD) wdL = max(SCR_WD-x, 0);
  for(j=0; j<ySize8; j++) {
    gotoXY(x, row+j);
    setDat();
    startCS();
    for(i=0; i<wdL; i++) sendData(0);
    for(i=0; i<wd; i++)  sendData(pgm_read_byte(font+idx+i*ySize8+j));
    for(i=0; i<wdR; i++) sendData(0);
    stopCS();
  }
  return wd+wdL+wdR;
}
// ---------------------------------
unsigned char N5110_SPI::convertPolish(unsigned char _c)
{
  unsigned char pl, c = _c;
  if(c==196 || c==197 || c==195) {
	  dualChar = c;
    return 0;
  }
  if(dualChar) { // UTF8 coding
    switch(_c) {
      case 133: pl = 1+9; break; // '¹'
      case 135: pl = 2+9; break; // 'æ'
      case 153: pl = 3+9; break; // 'ê'
      case 130: pl = 4+9; break; // '³'
      case 132: pl = dualChar==197 ? 5+9 : 1; break; // 'ñ' and '¥'
      case 179: pl = 6+9; break; // 'ó'
      case 155: pl = 7+9; break; // 'œ'
      case 186: pl = 8+9; break; // 'Ÿ'
      case 188: pl = 9+9; break; // '¿'
      //case 132: pl = 1; break; // '¥'
      case 134: pl = 2; break; // 'Æ'
      case 152: pl = 3; break; // 'Ê'
      case 129: pl = 4; break; // '£'
      case 131: pl = 5; break; // 'Ñ'
      case 147: pl = 6; break; // 'Ó'
      case 154: pl = 7; break; // 'Œ'
      case 185: pl = 8; break; // ''
      case 187: pl = 9; break; // '¯'
      default:  return c; break;
    }
    dualChar = 0;
  } else   
  switch(_c) {  // Windows coding
    case 165: pl = 1; break; // ¥
    case 198: pl = 2; break; // Æ
    case 202: pl = 3; break; // Ê
    case 163: pl = 4; break; // £
    case 209: pl = 5; break; // Ñ
    case 211: pl = 6; break; // Ó
    case 140: pl = 7; break; // Œ
    case 143: pl = 8; break; // 
    case 175: pl = 9; break; // ¯
    case 185: pl = 10; break; // ¹
    case 230: pl = 11; break; // æ
    case 234: pl = 12; break; // ê
    case 179: pl = 13; break; // ³
    case 241: pl = 14; break; // ñ
    case 243: pl = 15; break; // ó
    case 156: pl = 16; break; // œ
    case 159: pl = 17; break; // Ÿ
    case 191: pl = 18; break; // ¿
    default:  return c; break;
  }
  return pl+'~'+1;
}
// ---------------------------------
