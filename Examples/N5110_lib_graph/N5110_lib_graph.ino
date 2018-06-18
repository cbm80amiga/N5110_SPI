// (C)2016-18 Pawel A. Hernik
// N5110 LCD pinout from left:
// #1 RST      - Pin  9 or any digital
// #2 CS/CE    - Pin 10 or any digital
// #3 DC       - Pin  8 or any digital
// #4 MOSI/DIN - Pin 11 / SPI
// #5 SCK/CLK  - Pin 13 / SPI
// #6 VCC 3.3V
// #7 LIGHT (200ohm to GND)
// #8 GND

#include "N5110_SPI.h"
// define USESPI in above header for HW SPI version
N5110_SPI lcd(9,10,8); // RST,CS,DC

#if USESPI==1
#include <SPI.h>
#endif

//#include "fonts_all.h"
#include "small5x7bold_font.h"
#include "term8x14_font.h"
#include "digits4x7_font.h"

const uint8_t npn [] PROGMEM = {28,4,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x40, 0x20, 0x20, 0x20, 0x10, 0x10, 0x10,
0x10, 0x10, 0x10, 0x20, 0x20, 0xA0, 0x40, 0xA0, 0x10, 0x0F, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80,
0xE0, 0x9C, 0x82, 0x81, 0x80, 0x80, 0x80, 0x80, 0xFE, 0xFE, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02,
0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x1C, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x38, 0x40, 0x80,
0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x02, 0x04, 0x08, 0x90, 0xA0, 0xC0, 0xF0, 0x00, 0x00, 0x00,
0x80, 0x40, 0x38, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x04,
0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 0x05, 0x02, 0x05, 0x08, 0xF0, 0x00, 0x00
};

const uint8_t ind [] PROGMEM = {27,1,
0x40, 0x40, 0x40, 0x78, 0x04, 0x02, 0x02, 0x04, 0x78, 0x04, 0x02, 0x02, 0x04, 0x78, 0x04, 0x02,
0x02, 0x04, 0x78, 0x04, 0x02, 0x02, 0x04, 0x78, 0x40, 0x40, 0x40, 
};

void setup() 
{
  Serial.begin(9600);
  lcd.init();
}

// --------------------------------------------------------------------------
byte scr[84*6];  // full frame buffer - for demo
byte scrWd = 84;
byte scrHt = 6;

void clrBuf()
{
  int ii=0;
  for(int i=0;i<scrWd*scrHt;i++) scr[ii++]=0;
}
// --------------------------------------------------------------------------
void drawPixel(int16_t x, int16_t y, uint16_t color) 
{
  if(x<0 || x>=scrWd || y<0 || y>=scrHt*8) return;
  switch (color) {
    case 1: scr[x+(y/8)*scrWd] |=  (1 << (y&7)); break;
    case 0: scr[x+(y/8)*scrWd] &= ~(1 << (y&7)); break; 
    case 2: scr[x+(y/8)*scrWd] ^=  (1 << (y&7)); break; 
  }
}
// --------------------------------------------------------------------------

char buf[40];
void loop() 
{
  int i,j;
  lcd.clrScr();
  lcd.setFont(Term8x14PL);
  lcd.printStr(55, 3, "Cnt:");
  lcd.drawBitmap(npn, 3, 0);
  lcd.drawBitmap(ind, 50, 0);
  lcd.fillWin(40,1,44,2,0xaa);
  lcd.setFont(Small5x7PLBold);
  lcd.printStr(0,2,"B");
  lcd.printStr(30,0,"E");
  lcd.printStr(30,3,"C");
  lcd.printStr(44,0,"0");
  lcd.printStr(78,0,"1");
  lcd.setFont(Digits4x7);
  lcd.setDigitMinWd(4);
  
  // 54x16 pixels graph
  scrWd = 54;
  scrHt = 2;
  for(j=0;j<400;j++) {
    clrBuf();  
    for(i = 0;i<scrWd;i++) {
      int y = sin(i/5.0)*sin(j/10.0+i/10.0)*8+8;
      drawPixel(i,y,1);
    }
    lcd.drawBuf(scr,0,4,scrWd,scrHt);
    snprintf(buf,39,"%06d",random(1,1000000));
    if((j%4)==0) lcd.printStr(55,5,buf);
    delay(20);
  }
  
  // full screen graph
  scrWd = 84;
  scrHt = 6;
  for(j=0;j<400;j++) {
    clrBuf();  
    for(i = 0;i<scrWd;i++) {
      int y = sin(i/10.0)*sin(j/10.0+i/20.0)*24+24;
      drawPixel(i,y,1);
      drawPixel(i,y+1,1);
      drawPixel(i+1,y,1);
      drawPixel(i+1,y+1,1);
      drawPixel(i,0,1);
      drawPixel(i,scrHt*8-1,1);
    }
    for(i=1;i<scrHt*8-1;i++) {
      drawPixel(0,i,1);
      drawPixel(scrWd-1,i,1);
    }
    lcd.drawBuf(scr,0,0,scrWd,scrHt);
    lcd.setFont(Small5x7PLBold);
    lcd.printStr(ALIGN_CENTER,1,"GRAPH");
    delay(20);
  }

}
