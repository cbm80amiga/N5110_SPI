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
#include "small4x7_font.h"
#include "digits4x7_font.h"

const uint8_t ind [] PROGMEM = {27,1,
0x40, 0x40, 0x40, 0x78, 0x04, 0x02, 0x02, 0x04, 0x78, 0x04, 0x02, 0x02, 0x04, 0x78, 0x04, 0x02,
0x02, 0x04, 0x78, 0x04, 0x02, 0x02, 0x04, 0x78, 0x40, 0x40, 0x40, 
};

void setup() 
{
  Serial.begin(9600);
  lcd.init();
  lcd.setDigitMinWd(0);
}

// --------------------------------------------------------------------------
char buf[40];
void loop() 
{
  lcd.clrScr();
  lcd.setFont(Small5x7PLBold);
  lcd.printStr(ALIGN_LEFT, 0, "Align left");
  lcd.drawBitmap(ind, ALIGN_LEFT, 1);
  lcd.printStr(ALIGN_RIGHT, 2, "Align right");
  lcd.drawBitmap(ind, ALIGN_RIGHT, 3);
  lcd.printStr(ALIGN_CENTER, 4, "Align center");
  lcd.drawBitmap(ind, ALIGN_CENTER, 5);
  delay(4000);
  
  lcd.clrScr();
  lcd.setFont(Small4x7PL);
  lcd.setFontMinWd(0); // default proportional font
  lcd.printStr(0, 0, "abcdefghijklmnopqrtsuvwxyz");
  lcd.setFontMinWd(4); // force fixed width
  lcd.printStr(0, 1, "abcdefghijklmnopqrtsuvwxyz");
  lcd.setFontMinWd(5);
  lcd.printStr(0, 2, "abcdefghijklmnopqrtsuvwxyz");
  lcd.setFontMinWd(0);
  
  for(int i=0;i<50;i++) {
    snprintf(buf,19,"%05u",(unsigned int) random(1,100000));
    //lcd.fillWin(0,3,84,3,0xaa);
    lcd.fillWin(0,3,84,3,0x00);
    lcd.setFont(Digits4x7);
    lcd.setDigitMinWd(0);
    lcd.printStr(ALIGN_LEFT, 3, buf);
    lcd.printStr(ALIGN_RIGHT, 3, buf);
    lcd.setDigitMinWd(4);
    lcd.printStr(ALIGN_LEFT, 4, buf);
    lcd.printStr(ALIGN_RIGHT, 4, buf);
    lcd.setDigitMinWd(5);
    lcd.printStr(ALIGN_LEFT, 5, buf);
    lcd.printStr(ALIGN_RIGHT, 5, buf);
    lcd.setFont(Small4x7PL);
    lcd.setFontMinWd(0); // default proportional font
    lcd.printStr(ALIGN_CENTER, 3, "prop");
    lcd.printStr(ALIGN_CENTER, 4, "fix4");
    lcd.printStr(ALIGN_CENTER, 5, "fix5");
    delay(400);
  }
  delay(1000);
}
