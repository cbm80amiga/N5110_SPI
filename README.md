# N5110_SPI
Minimalistic Arduino Nokia 5110 LCD library

Doesn't use RAM as framebuffer - renders everything directly to LCD framebuffer

## Connections

|N5110 pin|Pin name| Arduino|
|--|--|--|
|#1| RST    |Pin  9 or any digital|
|#2| CS/CE  |Pin 10 or any digital|
|#3| DC     |Pin  8 or any digital|
|#4| MOSI/DIN |Pin 11 / SPI|
|#5| SCK/CLK  |Pin 13 / SPI|
|#6| VCC| 3.3V|
|#7| LIGHT| 200ohm to GND|
|#8 |GND|GND|
