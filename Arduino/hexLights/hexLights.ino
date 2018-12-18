/*NOTES TO SELF:
  - Try to create brightness feed and see if it'll work
  - OTHER SENSORS:
    - (DONE) Soil
    - Infrared
    - Sound
    - Light
    

*/



// Visit io.adafruit.com for an account and your IO_KEY#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__

  #include <avr/power.h>
#endif

#define PIXEL_PIN 14
#define PIXEL_COUNT 74
#define PIXEL_TYPE NEO_GRB + NEO_KHZ800
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
#include "AdafruitIO_WiFi.h"
int pixelColor = 0;
int valueRainbow = 3;
float oldBright = 0;


//allConnections
String line;
String data;
String hosCode;
String rmNum;
int color = 0xFFFFFF;
bool autoColor = true;
bool autoBright = true;
int age = 25;
int temp = 30;
int bright = 255;
bool aTemp = true;
bool aAutoBright = true;
bool aAutoColor = true;

void setup() {

//Allow the NodeMCU to talk to the computer
  Serial.begin(9600);

  fetch();
  assign();
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("strip begun");
  colorWipe(0xFFFFFF, 35);
  
}

void loop() {
 grabInt("color");
 grabInt("brightness");
 if (oldBright != bright){
  Serial.print("New brightness: ");
  Serial.println(bright);
  oldBright = bright;
  strip.setBrightness(bright);
  strip.show();
 }

 if (pixelColor != color){
  Serial.print("New color: ");
  Serial.println(color);
  pixelColor = color;
  for(int i=0; i<PIXEL_COUNT; ++i) {
    pixels.setPixelColor(i, color);
    pixelColor = (i, color);
  }
  colorWipe(pixelColor, 35);
 }
}


  
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all color
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
