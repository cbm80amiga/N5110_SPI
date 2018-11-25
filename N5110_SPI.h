// low resource Nokia 5110 driver
// 2014-2017 by Pawel A. Hernik

// N5110 module from left (defaults):
// #1 RST      - Pin 9
// #2 CS/CE    - Pin 10
// #3 DC       - Pin 8
// #4 MOSI/DIN - Pin 11
// #5 SCK/CLK  - Pin 13
// #6 VCC 3.3-5.0V
// #7 LIGHT (200ohm to GND)
// #8 GND

#ifndef _N5110_SPI_H
#define _N5110_SPI_H 

// define USESPI for hw SPI version, instead sw bit banging will be used
#define USESPI 1

#include <Arduino.h>
#include <avr/pgmspace.h>

#define SCR_WD  84
#define SCR_HT  48

#define ALIGN_LEFT    0
#define ALIGN_RIGHT  -1 
#define ALIGN_CENTER -2

// PCD8544 Commandset
// ------------------
// General commands
#define PCD8544_POWERDOWN         0x04
#define PCD8544_ENTRYMODE         0x02
#define PCD8544_EXTINSTRUCTION    0x01
#define PCD8544_DISPLAYBLANK      0x00
#define PCD8544_DISPLAYNORMAL     0x04
#define PCD8544_DISPLAYALLON      0x01
#define PCD8544_DISPLAYINVERTED   0x05
// Normal instruction set
#define PCD8544_FUNCTIONSET       0x20
#define PCD8544_DISPLAYCONTROL    0x08
#define PCD8544_SETYADDR          0x40
#define PCD8544_SETXADDR          0x80
// Extended instruction set
#define PCD8544_SETTEMP           0x04
#define PCD8544_SETBIAS           0x10
#define PCD8544_SETVOP            0x80
// Display presets
#define DEFAULT_BIAS              0x04 // 0-7 0x03?
#define DEFAULT_TEMP              0x00 // 0-3 0x02?
#define DEFAULT_CONTRAST          0x30 // 0-127 0x46?


// ---------------------------------
class N5110_SPI {
public:
	N5110_SPI(byte rst, byte cs, byte dc);
	N5110_SPI(byte rst, byte cs, byte dc, byte din, byte clk);
	inline void setDat() {digitalWrite(dcPin, HIGH); }
	inline void setCmd() { digitalWrite(dcPin, LOW); }
	inline void startCS() { digitalWrite(csPin, LOW); }
	inline void stopCS() { digitalWrite(csPin, HIGH); }

  void init();
	inline void sendData(byte data);
	inline void sendData(const uint8_t *data, int cnt);
	inline void sendDataF(const uint8_t *data, int cnt);
	void gotoXY(byte x, byte y);
  void sleep(bool mode);
  void setContrast(byte val);
  void setBias(byte val);
  void setTemp(byte val);
  void displayMode(byte val);  // val=PCD8544_DISPLAY????
	void clrScr();
	int drawBuf(const uint8_t *bmp, int x, uint8_t y8, uint8_t wd, uint8_t ht8);
  int drawBitmap(const uint8_t *bmp, int x, uint8_t y8, uint8_t wd, uint8_t ht8);
	int drawBitmap(const uint8_t *bmp, int x, uint8_t y8);
	int fillWin(int x, uint8_t y8, uint8_t wd, uint8_t ht8, uint8_t data);

  void setFont(const uint8_t* f);
	void setCR(uint8_t _cr) { cr = _cr; }
	void setInvert(uint8_t _inv) { invertCh = _inv; }
	void setFontMinWd(uint8_t wd) { minCharWd = wd; }
	void setCharMinWd(uint8_t wd) { minCharWd = wd; }
	void setDigitMinWd(uint8_t wd) { minDigitWd = wd; }
	int printStr(int x, uint8_t y8, char *txt);
	int printChar(int x, uint8_t y8, uint8_t ch);
  int charWidth(uint8_t _ch, bool last=true);
  int strWidth(char *txt);
	unsigned char convertPolish(unsigned char _c);

public:
  uint8_t csPin  = 10;
  uint8_t rstPin = 9;
  uint8_t dcPin  = 8;
  uint8_t dinPin = 11;
  uint8_t clkPin = 13;
  //uint8_t ledPin = 6;
   
private:
  const uint8_t* font; 
  uint8_t xSize;
	uint8_t ySize;
	uint8_t ySize8;
	uint8_t firstCh;
	uint8_t lastCh;
	uint8_t minCharWd;
	uint8_t minDigitWd;
  uint8_t cr;  // carriage return mode for printStr
	uint8_t dualChar;
  uint8_t invertCh;
	
};
#endif

