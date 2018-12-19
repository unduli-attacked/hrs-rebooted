#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
//TODO try just straight-up including allConnections.ino and see if it breaks
#ifdef __AVR__

#include <avr/power.h>
#endif

//TODO there's no lights on this. why is this code here?
#define PIXEL_PIN 14
#define PIXEL_COUNT 74
#define PIXEL_TYPE NEO_GRB + NEO_KHZ800
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
#include "AdafruitIO_WiFi.h"
int currentColor = 16777215; //TODO hex, decimal, or binary. pick ONE and stick to it
//TODO add comments to at least group this chaos
bool tooHigh = false;
bool tooLow = false;
bool nighttime = false;
int nextOne;
int zero;
int one;
int two;
int three;
int four;
int five;
int highCount = 0;
int lowCount = 0;
int lastTime;
int oldColor = 0;
int high = 95;
int low = 52;
int hr;
int mi;
int sec;
String month; //TODO make an int month and have an array for month names?
int day;
int year;
bool more0Added = false;
bool less0Sub = false;
bool leapYear;
int lastChange = 0;
int monthLength;
bool addDay = false;
bool subDay = false;
int pthr; //TODO jfc what even are variable names
int timeZone = -7;
int sunsetHr = 19;
int sunsetMin = 0;
int sunriseHr = 6;
int sunriseMin = 30;
bool nightChange = false;
int minOfDay;
int lastBright = 20000; //TODO what is this and how is it different from oldBright
float oldBright = 255;
int lastLocal = millis();
bool first = true;

int lightBlue = 0x68c2f0; //TODO any particular reason for these defaults?
int mediumBlue = 0x00bfff;
int darkBlue = 0x0000ff;
int lightYellow = 0xfedf33;
int mediumYellow = 0xfed911;
int darkYellow = 0xf5d50b;
int white = 0xffffff;
int night = 0x441200;

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
int BPM;
int tzMins = -420; //TODO this is a different name from what's actually in allConnections. what is u doing
bool tzError = false;

void setup() {

  //Allow the NodeMCU to talk to the computer
  Serial.begin(9600);

  WiFiManager wifiManager;
  wifiManager.autoConnect("connectorAP");
  Serial.println("Connected \n");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  fetch(); //TODO again with the inside assign()
  assign();

  tzMins = grabInt("timeZone");
  if (tzMins % 60 == 0) {
    timeZone = tzMins / 60;
  } else {
    int adjTzMins = tzMins - 30;
    if (adjTzMins % 60 == 0) {
      timeZone = adjTzMins / 60;
    } else {
      adjTzMins = tzMins - 45; //TODO 15mins exist. deal with it
      if (adjTzMins % 60 == 0) {
        timeZone = adjTzMins / 60;
      } else {
        Serial.println("Time zone error");
        tzError = true; //TODO is there somehting we do with a tzError? or is it just kinda there?
      }
    }
  }

  //TODO wtf does this do. wtf is it for
  if (timeZone > 0) {
    if (hr >= 24 - timeZone) {
      addDay = true;
    } else if (hr >= 0 && more0Added == true) {
      subDay = true;
      more0Added = false;
    }
  } else if (timeZone < 0) {
    Serial.println("tZ less than 0");
    if (hr >= 0) {
      Serial.println("hr greater than 0");
      if (hr <= timeZone * -1) {
        Serial.println("hr less than 7"); //TODO its not always 7 u dumbass
        subDay = true;
      }

    } else if (hr >= (timeZone * -1) && less0Sub == true) {
      addDay = true;
      less0Sub = false;
    }
  }
  lastTime = millis();
}

void loop() {
  if (millis() - 30000 >= lastLocal) {
    localTime();
    lastLocal = millis();
  }
  if (hr >= sunsetHr && mi >= sunsetMin) {
    nighttime = true;
  } else if (hr <= sunriseHr && mi <= sunriseMin) {
    nighttime = true;
  } else {
    nighttime = false;
  }

  daylight();
  bpm();
  autoColor = grabBool("autoColor");
  autoBright = grabBool("autoBright");
  ages();
  aAutoColor = grabAdmin("autoColor");
  aAutoBright = grabAdmin("autoBright");


  if (autoColor == true) {
    if (currentColor != oldColor) { //TODO add as an && to original if
      if (lastTime - millis() >= 1000) {//TODO add as an && to original if
        Serial.print("Old color: ");
        Serial.println(oldColor);
        String hexCol = hexa(currentColor);//TODO can this be done in an allConnections function?
        sennd("color", hexCol);
        Serial.print("New color: ");
        Serial.println(currentColor);
        oldColor = currentColor;
        lastTime = millis();
      } else {
        Serial.println("waiting"); //TODO unimportant, delete
      }
    } else {
      //Serial.println("same color");
    }
  }
}

void bpm() {

  //Extract the number
  BPM = grabInt("bpm");

  //Print out the value
  Serial.print("received <- ");
  Serial.println(BPM);

  if (BPM > high) {
    Serial.println("too high");
    nightChange = true; //TODO why does it become night if someone's panicking?????
    lowCount = 0;
    if (BPM <= high + 5) {
      currentColor = lightBlue;
      highCount = 1;
      Serial.println("New color: light blue");

    } else if (BPM > high + 5 && BPM <= high + 17) {
      currentColor = mediumBlue;
      highCount = 2;
      Serial.println("New color: medium blue");
    } else if (BPM > high + 17) {
      currentColor = darkBlue;
      highCount = 3;
      Serial.println("New color: dark blue");
    }
  } else if (BPM < low) {
    Serial.println("too low");
    nightChange = true;
    highCount = 0;
    if (BPM >= low - 5) {
      currentColor = lightYellow;
      lowCount = 1;
      Serial.println("New color: light yellow");
    } else if (BPM < low - 5 && BPM >= low - 17) {
      currentColor = mediumYellow;
      lowCount = 2;
      Serial.println("New color: medium yellow");
    } else if (BPM < low - 17) {
      currentColor = darkYellow;
      lowCount = 3;
      Serial.println("New color: dark yellow");
    }

  } else {
    nightChange = false;
    Serial.println("just right");
    //TODO so the whole highCount/lowCount thing kinda looks like the i in pid, so why not go all the way and make at least pi 
    //for heart rate by adding blue/yellow depending on output
    if (highCount == 3) {
      currentColor = mediumBlue;
      highCount = 2;
      Serial.println("New color: medium blue");
    } else if (highCount == 2) {
      currentColor = lightBlue;
      highCount = 1;
      Serial.println("New color: light blue");
    } else if (highCount == 1) {
      currentColor = white;
      highCount = 0;
      Serial.println("New color: white");
    } else if (lowCount == 3) {
      currentColor = mediumYellow;
      lowCount = 2;
      Serial.println("New color: medium yellow");
    } else if (lowCount == 2) {
      currentColor = lightYellow;
      lowCount = 1;
      Serial.println("New color: light yellow");
    } else if (lowCount == 1) {
      currentColor = white;
      lowCount = 0;
      Serial.println("New color: white");
    } else if (lowCount == 0 && highCount == 0) {
      localTime();

      if (nighttime == true) {
        currentColor = night;
      } else {
        currentColor = white;
      }
    }
    Serial.print("The low count is: ");
    Serial.println(lowCount);
    Serial.print("The high count is: ");
    Serial.println(highCount);
  }
}

void ages() { //TODO rename to rep. the fact that it's tied to bpm

  //Extract the number
  age = grabInt("age");

  //Print out the value
  Serial.print("received <- ");
  Serial.println(age);
  if (age == 1) {
    high = 139;
    low = 95;
  } else if (age == 2 or age == 3) {
    high = 125;
    low = 85;
  } else if (age == 4 or age == 5) {
    high = 117;
    low = 74;
  } else if (age >= 6 && age <= 8) {
    high = 106;
    low = 66;
  } else if (age >= 9 && age <= 11) {
    high = 103;
    low = 61;
  } else if (age >= 12 && age <= 15) {
    high = 99;
    low = 57;
  } else if (age >= 16 && age <= 19) {
    high = 99;
    low = 52;
  } else if (age >= 20 && age <= 39) {
    high = 95;
    low = 52;
  } else if (age >= 40 && age <= 79) {
    high = 92;
    low = 50;
  } else if (age >= 80) {
    high = 94;
    low = 51;
  }
}

void daylight() {
  float a; //TODO hun. why. with. the. names.
  float b;
  float c;
  float m;
  float d;
  bool squared;
  float xr = 390;
  float yr = 201.853;
  float xs = 1140;
  float ys = 168.91;
  float xp = 720;
  float yp = 255;
  float calc5;
  //TODO update with what I have in desmos
  if (minOfDay <= 1140 && minOfDay >= 390) {
    a = -0.00048804;
    b = 0.702778;
    c = 2;
    squared = true;
  } else if (minOfDay < 390 && minOfDay > 270) {
    d = (((2 * xr) - (xr * yr)) / 120) + yr;
    m = (yr - 2) / 120;
    squared = false;
  } else if (minOfDay > 1140 && minOfDay < 1260) {
    d = (((xs * ys) - (2 * xs)) / 120) + ys;
    m = (2 - ys) / 120;
    squared = false;
  } else {
    a = 0;
    b = 0;
    c = 2;
    squared  = true;
  }
  if (squared  == true) {
    float calc1 = minOfDay * minOfDay;
    float calc2 = a * calc1;
    float calc3 = b * minOfDay;
    float calc4 = calc2 + calc3;
    float calc5 = calc4 + c;
  } else {
    float calc1 = m * minOfDay;
    float calc5 = calc1 + d;
  }
  bright = calc5;
  if (oldBright != bright) {
    if (lastBright - millis() >= 1000) {
      if (first == true) { //TODO why is this even here?
        Serial.print("Old brightness: ");
        Serial.println(oldBright);
        sennd("brightness", String(bright)); //TODO why is it a string?
        Serial.print("New brightness: ");
        Serial.println(255); //TODO why are you saying something that's not 100% true?
        oldBright = 255;
        lastBright = millis();
        first = false;
      } else {
        Serial.print("Old brightness: ");
        Serial.println(oldBright);
        sennd("brightness", String(bright));
        Serial.print("New brightness: ");
        Serial.println(bright);
        oldBright = bright;
        lastBright = millis();
      }
    } else {
      Serial.println("waiting"); //TODO unimportant
    }
  }
}

String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) {
    Serial.println("connection failed, retrying...");
  }

  client.print("HEAD / HTTP/1.1\r\n\r\n");

  while (!!!client.available()) {
    yield();
  }

  while (client.available()) {
    if (client.read() == '\n') {//TODO wow this is lovely
      if (client.read() == 'D') {
        if (client.read() == 'a') {
          if (client.read() == 't') {
            if (client.read() == 'e') {
              if (client.read() == ':') {
                client.read();
                String theDate = client.readStringUntil('\r');
                client.stop();
                return theDate;
              }
            }
          }
        }
      }
    }
  }
}

void localTime() {
  String dt = getTime();
  Serial.println(dt);
  int dtLength = dt.length() + 1;
  char dtArray[dtLength];
  dt.toCharArray(dtArray, dtLength);
  int da1 = dtArray[5] - '0';
  int da2 = dtArray[6] - '0';
  String mo1 = String(dtArray[8]);
  String mo2 = String(dtArray[9]);
  String mo3 = String(dtArray[10]);
  int yr1 = dtArray[12] - '0';
  int yr2 = dtArray[13] - '0';
  int yr3 = dtArray[14] - '0';
  int yr4 = dtArray[15] - '0';
  int hr1 = dtArray[17] - '0';
  int hr2 = dtArray[18] - '0';
  int min1 = dtArray[20] - '0';
  int min2 = dtArray[21] - '0';
  int sec1 = dtArray[23] - '0';
  int sec2 = dtArray[24] - '0';
  month = mo1 + mo2 + mo3;
  yr1 = yr1 * 1000;
  yr2 = yr2 * 100;
  yr3 = yr3 * 10;
  year = yr1 + yr2 + yr3 + yr4;
  da1 = da1 * 10;
  day = da1 + da2;
  hr1 = hr1 * 10;
  hr = hr1 + hr2;
  min1 = min1 * 10;
  mi = min1 + min2;
  sec1 = sec1 * 10;
  sec = sec1 + sec2;
  
  //TODO seperate into a whole other function
  if (tzError == false) {
    if (timeZone > 0) {
      if (hr >= 24 - timeZone) {
        Serial.println("day plus");
        day++;
        lastChange = millis();
        more0Added = true; //TODO tf does this even mean
      } else if (hr >= 0 && more0Added == true) {
        day--;
        lastChange = millis();
        more0Added = false;
      }
    } else if (timeZone < 0) {
      Serial.println("tZ less than 0");
      if (hr >= 0 && hr <= timeZone * -1) {
        day--;
        lastChange = millis();
        less0Sub = true;
      } else if (hr >= (timeZone * -1) && less0Sub == true) {
        Serial.println("day plus");
        day++;
        lastChange = millis();
        less0Sub = false;
      }
    }

    if (year % 4 == 0) {
      leapYear = true;
    } else {
      Serial.println("Leap year false");
      leapYear = false;
    }
    if (month == "Sep" or month == "Apr" or month == "Jun" or month == "Nov") {
      monthLength = 30;
      Serial.println("month length 30");
    } else if (month == "Feb" && leapYear == false) {
      monthLength = 29;
    } else if (month == "Feb" && leapYear == true) {
      monthLength = 30; //TODO bruh is this even right?
    } else {
      monthLength = 31;
    }
    if (day == monthLength) {
      if (timeZone > 0) {
        if (hr == 24 - timeZone && mi == 0 && millis() - lastChange > 60000) {
          nextMonth();
          lastChange = millis();
          more0Added = true;
        } else if (hr == 0 && mi == 0 && millis() - lastChange > 60000 && more0Added == true) {
          previousMonth();
          lastChange = millis();
          more0Added = false;
        }
      }
    } else if (timeZone < 0) {
      if (hr == 0 && mi == 0 && millis() - lastChange > 60000) {
        previousMonth();
        lastChange = millis();
        less0Sub = true;
      } else if (hr == (timeZone * -1) && mi == 0 && millis() - lastChange > 60000 && less0Sub == true) {
        nextMonth();
        lastChange = millis();
        less0Sub = false;
      }
    }
    //TODO pretty sure this is 100% redundant
    if (addDay == true) {
      Serial.println("day plus");
      day++;
      addDay = false;
      lastChange == millis();
    } else if (subDay == true) {
      Serial.println("subDay true");
      day--;
      subDay = false;
      lastChange == millis();
    }
    if (timeZone > 0) {
      if (hr >= 24 - timeZone) {
        pthr = hr + timeZone; //TODO pre-timezone hour. it stands for pre-timezone hour. W H Y
        pthr = pthr - 24;
        hr = 0 + pthr;
      } else {
        hr = hr + timeZone;
      }
    } else if (timeZone < 0) {
      Serial.println("tZ less than 0");
      if (hr >= 24 + timeZone or hr == 0 or hr <= timeZone * -1) {
        Serial.println("Alteration required");
        pthr = hr + timeZone;
        pthr = pthr + 24;
        hr = pthr;
      } else {
        Serial.println("No alt");
        hr = hr + timeZone;
      }
    }
  }
  Serial.print("Local date: ");
  Serial.print(day);
  Serial.print(" ");
  Serial.print(month);
  Serial.print(" ");
  Serial.println(year);
  Serial.print("Local time: ");
  Serial.print(hr);
  Serial.print(":");
  Serial.print(mi);
  Serial.print(":");
  Serial.println(sec);
  minOfDay = (hr * 60) + mi;
  delay(1000);
}

void nextMonth() {  //TODO f
  if (month == "Jan") {
    month = "Feb";
  } else if (month == "Feb") {
    month = "Mar";
  } else if (month == "Mar") {
    month = "Apr";
  } else if (month == "Apr") {
    month = "May";
  } else if (month == "May") {
    month = "Jun";
  } else if (month == "Jun") {
    month = "Jul";
  } else if (month == "Jul") {
    month = "Aug";
  } else if (month == "Aug") {
    month = "Sep";
  } else if (month == "Sep") {
    month = "Oct";
  } else if (month == "Oct") {
    month = "Nov";
  } else if (month == "Nov") {
    month = "Dec";
  } else if (month == "Dec") {
    month = "Jan";
    year++;
  }
}

void previousMonth() { //TODO mega f
  if (month == "Jan") {
    month = "Dec";
    year--;
  } else if (month == "Feb") {
    month = "Jan";
  } else if (month == "Mar") {
    month = "Feb";
  } else if (month == "Apr") {
    month = "Mar";
  } else if (month == "May") {
    month = "Apr";
  } else if (month == "Jun") {
    month = "May";
  } else if (month == "Jul") {
    month = "Jun";
  } else if (month == "Aug") {
    month = "Jul";
  } else if (month == "Sep") {
    month = "Aug";
  } else if (month == "Oct") {
    month = "Sep";
  } else if (month == "Nov") {
    month = "Oct";
  } else if (month == "Dec") {
    month = "Nov";
  }
}


int deca (int hex1, int hex2, int hex3, int hex4, int hex5, int hex6) { //TODO does this fucker actually work?
  int fi = hex1;
  int fo = hex2;
  int th = hex3;
  int tw = hex4;
  int o = hex5;
  int ze = hex6;
  int p1 = fi * (16 * 16 * 16 * 16 * 16);
  int p2 = fo * (16 * 16 * 16 * 16);
  int p3 = th * (16 * 16 * 16);
  int p4 = tw * (16 * 16);
  int p5 = o * (16);
  int p6 = ze;
  int fin = p1 + p2 + p3 + p4 + p5 + p6;
  return fin;
}

String hexa (int dec) { //TODO how about this one?
  int zer = dec % 16; //TODO ah yes. just cut off the last letter of the word. genius
  int nextOne = (dec - zer) / 16;
  int o = nextOne % 16;
  nextOne = (nextOne - o) / 16;
  int tw = nextOne % 16;
  nextOne = (nextOne - tw) / 16;
  int thre = nextOne % 16;
  nextOne = (nextOne - thre) / 16;
  int fou = nextOne % 16;
  nextOne = (nextOne - fou) / 16;
  int fiv = nextOne % 16;
  char zero = check(zer);
  char one = check(o);
  char two = check(tw);
  char three = check(thre);
  char four = check(fou);
  char five = check(fiv);
  String fina = five + four + three + two + one + zero;
  return fina;

}

char check(int num) { //TODO is this even used?
  if (num <= 9) {
    return char(num);
  } else {
    if (num == 10) {
      return 'A';
    } else if (num == 11) {
      return 'B';
    } else if (num == 12) {
      return 'C';
    } else if (num == 13) {
      return 'D';
    } else if (num == 14) {
      return 'E';
    } else if (num == 15) {
      return 'F';
    }
  }
}

