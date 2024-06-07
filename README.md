<nobr>![Example](/example0.gif)
# HUB75 Matrix LED Sprites
This code can be used to drive two linked HUB75 Matrix LED 64x32 panels (128x32 but can be used with single 64x32 too) with an Arduino Nano, Pro Mini or Uno (ATMega 328p w/ 2kb RAM and 32kb flash). More powerfull hardware will work too !

The example code shows how to display 3 sprites at a time, repeating the exercice several times with various sprites (flickering demo is due to camera, real view is smooth and stable)

In order to be more efficient, row addressing is hard coded to A3~A0 Arduino pins (1/16 scanline)

A lot of sprites (currently about 13, only 14% flash !) can be used due to the fact that images are stored in flash (through PROGMEM directive compilation)

Note that sprites are, in fact, pseudo sprites because currently if there's collision, sprites are not "meld" (i.e background of collisoning sprite is black color, erasing other sprite pixels)

# Setup
You can use the bmp_to_3_bit_array.cc to generates code corresponding to image bitmap sprite

The format is restricted to 2 pixels per byte, MSB and LSB quartets codes color are : 0b_RGB_RGB (more information in source code)

Just copy/paste the source code in Arduino IDE, select Arduino Nano, Pro Mini or Uno board (ATMega328p), and flash it ! Probably compatible with others Arduino microcontrollers too

Souce code is documented so, read it and adjust it for your needs

# Example
You can see the sample code in action where panels are installed as marquee to a home made Arcade Cabinet (following gif is reduced to 5 fps for demonstration purpose, "real" view is estimated to about 50 fps !) :

<nobr>![Example](/example.gif)

# Dependencies
Code only uses FastGPIO library (https://www.arduino.cc/reference/en/libraries/fastgpio/) 

# Optimization
- Direct Arduino Port manipulation has to be managed in order to be more efficient but, hard coded pins have to be implemented !

# Limitations
Currently, 3 bits native depth colors is supported (white, black, red, green, blue, cyan, magenta, yellow)
