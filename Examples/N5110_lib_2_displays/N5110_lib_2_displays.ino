// (C)2016-18 Pawel A. Hernik
// N5110 from left:
// #1 RST      - Pin 9 / 6
// #2 CS/CE    - Pin 10 / 5
// #3 DC       - Pin 8
// #4 MOSI/DIN - Pin 11
// #5 SCK/CLK  - Pin 13
// #6 VCC 3.3V
// #7 LIGHT (200ohm to GND)
// #8 GND

#include "N5110_SPI.h"
// define USESPI in above header for HW SPI version

// DC,MOSI,SCK are shared
// RST and CS are separate for each display
N5110_SPI lcd(9,10,8); // RST,CS,DC
N5110_SPI lcd2(6,5,8); // RST,CS,DC


#if USESPI==1
#include <SPI.h>
#endif

#include "fonts_all.h"

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
  lcd2.init();
  // separate init is necessary only for my 2nd LCD
  lcd2.setContrast(0x38);
  lcd2.setBias(4);
  lcd2.setTemp(1);
}

// --------------------------------------------------------------------------
byte scr[84*6];  // full frame buffer
byte scrWd = 84;
byte scrHt = 6;

void clrBuf()
{
  int ii=0;
  //for(int i=0;i<scrWd*scrHt/4;i++) { scr[ii++]=0; scr[ii++]=0; scr[ii++]=0; scr[ii++]=0; }
  for(int i=0;i<scrWd*scrHt;i++) scr[ii++]=0;
}
// --------------------------------------------------------------------------
void drawPixel(int16_t x, int16_t y, uint16_t color) 
{
  if ((x < 0) || (x >= scrWd) || (y < 0) || (y >= scrHt*8)) return;

  switch (color) 
  {
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
  lcd.printStr(ALIGN_RIGHT, 3, "Cnt:");
  lcd.drawBitmap(npn, 3, 0);
  lcd.drawBitmap(ind, 50, 0);
  lcd.setFont(Small5x7PLBold);
  lcd.printStr(0,2,"B");
  lcd.printStr(30,0,"E");
  lcd.printStr(30,3,"C");
  lcd.printStr(44,0,"0");
  lcd.printStr(78,0,"1");
  lcd.setFont(Digits4x7);
  lcd.setDigitMinWd(4);
  lcd2.clrScr();
  lcd2.setFont(Term8x14PL);
  lcd2.printStr(ALIGN_CENTER, 0, "Graph");
  
  for(j=0;j<1000;j++) {
    scrWd = 54;
    scrHt = 2;
    clrBuf();  
    for(i = 0;i<scrWd;i++) {
      int y = sin(j/8.0+i/3.0)*8+8;
      drawPixel(i,y,1);
    }
    lcd.drawBuf(scr,0,4,scrWd,scrHt);
    snprintf(buf,99,"%06d",random(1,1000000));
    if((j%8)==0) lcd.printStr(ALIGN_RIGHT,5,buf);

    scrWd = 84;
    scrHt = 4;
    clrBuf();  
    for(i = 0;i<scrHt*8;i++) {
      drawPixel(0,i,1);
      drawPixel(scrWd-1,i,1);
    }
    for(i = 0;i<scrWd;i++) {
      drawPixel(i,0,1);
      drawPixel(i,scrHt*8-1,1);
      int y = sin(i/10.0)*sin(j/10.0+i/20.0)*scrHt*4+scrHt*4;
      drawPixel(i,y,1);
      drawPixel(i,y+1,1);
    }
    lcd2.drawBuf(scr,0,2,scrWd,scrHt);

    delay(20);
  }

}
