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

#include "small4x7_font.h"

void setup() 
{
  Serial.begin(9600);
  lcd.init();
}

// --------------------------------------------------------------------------

void loop() 
{
  lcd.clrScr();
  lcd.setFont(Small4x7PL);
  lcd.setDigitMinWd(8);
  lcd.printStr(0, 0, "Testing modes:");
  lcd.printStr(0, 1, "1.DISPLAYINVERTED");
  lcd.printStr(0, 2, "2.DISPLAYALLON");
  lcd.printStr(0, 3, "3.DISPLAYBLANK");
  lcd.printStr(0, 4, "4.DISPLAYNORMAL");
  delay(2000);
  lcd.displayMode(PCD8544_DISPLAYINVERTED);
  delay(2000);
  lcd.displayMode(PCD8544_DISPLAYALLON);
  delay(2000);
  lcd.displayMode(PCD8544_DISPLAYBLANK);
  delay(2000);
  lcd.displayMode(PCD8544_DISPLAYNORMAL);
  delay(2000);
}

