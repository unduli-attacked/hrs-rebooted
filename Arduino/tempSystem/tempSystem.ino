#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TMP006.h"
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
const int fanPin = D6;
int threshold = 30; //TODO this is set to something else in sensorArmband????

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
  // put your setup code here, to run once:
  pinMode(fanPin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(fanPin, LOW);

  WiFiManager wifiManager;
  wifiManager.autoConnect("fanAP");
  Serial.println("Connected \n");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //fetches the raw room and hospital assignments from the server
  fetch();
  //decodes the room and hospital assignments so the ESP can read them
  assign();
}

void loop() {
  //grabAdmin("temp"); //TODO delete
  temp = grabInt("temp"); //TODO where is the temp even set?
  aTemp = grabAdmin("temp");
  Serial.println(temp);
  if (aTemp == true) {
    if (temp > threshold) {
      digitalWrite(fanPin, HIGH);
      Serial.println("on");
    } else {
      digitalWrite(fanPin, LOW);
    }
  }
}
