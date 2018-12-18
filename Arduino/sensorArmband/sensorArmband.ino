
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Average.h>
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <Adafruit_NeoPixel.h>
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TMP006.h"

//for pulse
Average<float> ave(500);
Average<float> fin(3);

//for temp
int threshold = 32; //C
Adafruit_TMP006 tmp006;

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

//for pulse
int lastSaveTime = millis();
int currentTime = millis();
volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P = 512;                     // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 530;                // used to find instant moment of heart beat, seeded
volatile int amp = 0;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM
#define PROCESSING_VISUALIZER 1
#define SERIAL_PLOTTER  2

//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = LED_BUILTIN;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.


unsigned long lastTime; // used to time the Pulse Sensor samples
unsigned long thisTime; // used to time the Pulse Sensor samples
unsigned long fadeTime; // used to time the LED fade

WiFiClient client;
void setup()   {
  Serial.begin(9600);
  Serial.println("Calibrating....");
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  calibrate();

  if (! tmp006.begin()) {
    Serial.println("No sensor found");
    while (1);
  }

  Serial.println("Send s to enter sleep mode, or w to wake up.  Measurements are not updated while asleep!");


  pinMode(blinkPin, OUTPUT);        // pin that will blink to your heartbeat!
  pinMode(fadePin, OUTPUT);         // pin that will fade to your heartbeat
  // ADD THIS LINE IN PLACE OF THE interruptSetup() CALL
  lastTime = micros();              // get the time so we can create a software 'interrupt'

  WiFiManager wifiManager;
  wifiManager.autoConnect("sensorAP");
  Serial.println("Connected \n");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //fetches the raw room and hospital assignments from the server
  fetch();
  //decodes the room and hospital assignments so the ESP can read them
  assign();

  Serial.println(thresh);

  lastTime = micros();
}


void loop() {
  //Serial.println("In the loop");
  Signal = analogRead(A0);



  //Serial.println("serialOutput();");
  thisTime = micros();            // GET THE CURRENT TIME
  if (thisTime - lastTime > 117000) { // CHECK TO SEE IF 2mS HAS PASSED
    lastTime = thisTime;          // KEEP TRACK FOR NEXT TIME
    getPulse();                   //CHANGE 'ISR(TIMER2_COMPA_vect)' TO 'getPulse()' IN THE INTERRUPTS TAB!
  }

  if (QS == true) {    // A Heartbeat Was Found
    //Serial.println("QS == true");               // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRate = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    fadeTime = millis();    // Set the fade timer to fade the LED
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    QS = false;                      // reset the Quantified Self flag for next time
  }
}

void serialOutput() {  // Decide How To Output Serial.
  switch (outputType) {
    case PROCESSING_VISUALIZER:
      sendDataToSerial('S', Signal);     // goes to sendDataToSerial function
      break;
    case SERIAL_PLOTTER:  // open the Arduino Serial Plotter to visualize these data
      //Serial.println(BPM);
      currentTime = millis();
      if (currentTime - lastSaveTime > 10000) {
        Serial.println(BPM);
        Serial.println("saving");
        sennd("BPM", String(BPM));
        lastSaveTime = millis();
      }
      break;
    default:
      break;
  }

  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 'w') {
      Serial.println("Waking up!");
      tmp006.wake();
    }
    else if (c == 's') {
      Serial.println("Going to sleep!");
      tmp006.sleep();
    }
  }
  float temp = tmp006.readObjTempC();
  Serial.print("Object Temperature: "); Serial.print(temp); Serial.println("*C");
  sennd("temp", String(temp));

}


void getPulse() {                        // triggered when Timer2 counts to 124
  //Serial.println("getting pulse");
  cli();                                      // disable interrupts while we do this
  // read the Pulse Sensor
  sampleCounter += 117;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

  //  find the peak and trough of the pulse wave
  if (Signal < thresh && N > (IBI / 5) * 3) { // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T) {                       // T is the trough
      T = Signal;                         // keep track of lowest point in pulse wave
      //Serial.println("trough found");
    }
  }

  if (Signal > thresh && Signal > P) {        // thresh condition helps avoid noise
    P = Signal;                             // P is the peak
    //Serial.println("peak found");
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250) {                                  // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3) ) {
      Pulse = true;                               // set the Pulse flag when we think there is a pulse
      //digitalWrite(blinkPin, HIGH);               // turn on pin 13 LED
      //Serial.println("pin off");
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if (secondBeat) {                      // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                  // clear secondBeat flag
        for (int i = 0; i <= 9; i++) {       // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;
        }
      }

      if (firstBeat) {                       // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        sei();                               // enable interrupts again
        return;                              // IBI value is unreliable so discard it
      }


      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                  // clear the runningTotal variable

      for (int i = 0; i <= 8; i++) {          // shift data in the rate array
        rate[i] = rate[i + 1];                // and drop the oldest IBI value
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values
      BPM = 60000 / runningTotal;             // how many beats can fit into a minute? that's BPM!

      //Serial.println(BPM);
      QS = true;                              // set Quantified Self flag
      Serial.println("QS = true");
      serialOutput();
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }
  }

  if (Signal < thresh && Pulse == true) {  // when the values are going down, the beat is over
    //digitalWrite(blinkPin, LOW);           // turn off pin 13 LED
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp / 2 + T;                  // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500) {                          // if 2.5 seconds go by without a beat
    //thresh = 530;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

  sei();                                   // enable interrupts when youre done!
}// end isr

void ledFadeToBeat() {
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate, 0, 255); //  keep LED fade value from going into negative numbers!
  //analogWrite(fadePin, fadeRate);         //  fade LED
}

void serialOutputWhenBeatHappens() {
  switch (outputType) {
    case PROCESSING_VISUALIZER:    // find it here https://github.com/WorldFamousElectronics/PulseSensor_Amped_Processing_Visualizer
      sendDataToSerial('B', BPM);  // send heart rate with a 'B' prefix
      //sendDataToSerial('Q',IBI);   // send time between beats with a 'Q' prefix
      break;

    default:
      break;
  }
}

//  Sends Data to Pulse Sensor Processing App, Native Mac App, or Third-party Serial Readers.
void sendDataToSerial(char symbol, int data ) {
  Serial.print(symbol);
  Serial.println(data);
}

void calibrate() {
  int count = 0;
  int minOne;
  int maxOne;
  int minTwo;
  int maxTwo;
  int minThree;
  int maxThree;
  int minAverage;
  int maxAverage;
  int threshUnF;
  int threshUnFi;

  while (count < 500) {
    ave.push(analogRead(A0));
    delay(9);
    delay(1);
    count = count + 1;
  }

  minOne = ave.minimum();
  Serial.println(minOne);
  maxOne = ave.maximum();
  Serial.println(maxOne);
  count = 0;
  Serial.println("One done");
  while (count < 500) {
    ave.push(analogRead(A0));
    delay(9);
    delay(1);
    count = count + 1;
  }

  minTwo = ave.minimum();
  Serial.println(minTwo);
  maxTwo = ave.maximum();
  Serial.println(maxTwo);
  count = 0;
  Serial.println("Two done");

  while (count < 500) {
    ave.push(analogRead(A0));
    delay(9);
    delay(1);
    count = count + 1;
  }

  minThree = ave.minimum();
  Serial.println(minThree);
  maxThree = ave.maximum();
  Serial.println(maxThree);
  Serial.println("Three done");


  fin.push(minOne);
  fin.push(minTwo);
  fin.push(minThree);
  minAverage = fin.mean();
  Serial.println(minAverage);

  fin.push(maxOne);
  fin.push(maxTwo);
  fin.push(maxThree);
  maxAverage = fin.mean();
  Serial.println(maxAverage);
  Serial.println("Average done");


  threshUnF = maxAverage - minAverage;
  Serial.println(threshUnF);
  threshUnFi = threshUnF / 2;
  Serial.println(threshUnFi);
  thresh = threshUnFi + minAverage;
  Serial.println(thresh);
  Serial.println("Calibration done");

}

void assign() {
  int len = line.length() + 1;
  char asinmt[len];
  line.toCharArray(asinmt, len);
  String hos1 = String(asinmt[14]);
  String hos2 = String(asinmt[15]);
  String hos3 = String(asinmt[16]);
  String hos4 = String(asinmt[17]);
  String rm1 = String(asinmt[27]);
  rmNum = rm1;
  if (isDigit(asinmt[28])) {
    String rm2 = String(asinmt[28]);
    rmNum = rm1 + rm2;
    if (isDigit(asinmt[29])) {
      String rm3 = String(asinmt[29]);
      rmNum = rm1 + rm2 + rm3;
      if (isDigit(asinmt[30])) {
        String rm4 = String(asinmt[30]);
        rmNum = rm1 + rm2 + rm3 + rm4;
      }
    }
  }

  hosCode = hos1 + hos2 + hos3 + hos4;

  Serial.println();
  Serial.println(hosCode);
  Serial.println(rmNum);
}

void fetch() {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String ipString = WiFi.localIP().toString();
  char charBuf[ipString.length() + 1];

  // We now create a URI for the request
  String url = "/chips/";
  url = url + ipString.c_str();

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    line = client.readStringUntil('\r');
    Serial.println(line);
  }

  Serial.println();
  Serial.println("closing connection");

  int value = 0;
}

void sennd(String des, String toSend) {
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String ipString = WiFi.localIP().toString();
  char charBuf[ipString.length() + 1];

  // We now create a URI for the request
  String url = "/";
  url += hosCode;
  url += "/";
  url += rmNum;
  url += "/add.php";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }


  String dta = "d8a=" + toSend + "&ds=" + des;
  if (client.connect("teamcosmo.5gbfree.com", 80)) { // REPLACE WITH YOUR SERVER ADDRESS
    Serial.println("Sending........\n\n");
    client.println("POST /" + hosCode + "/" + rmNum + "/add.php HTTP/1.1");
    Serial.println("POST /" + hosCode + "/" + rmNum + "/add.php HTTP/1.1");
    client.println("Host: teamcosmo.5gbfree.com"); // SERVER ADDRESS HERE TOO
    Serial.println("Host: teamcosmo.5gbfree.com"); // SERVER ADDRESS HERE TOO
    client.println("Content-Type: application/x-www-form-urlencoded");
    Serial.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    Serial.print("Content-Length: ");
    client.println(dta.length());
    Serial.println(dta.length());
    client.println();
    Serial.println();
    client.print(dta);
    Serial.println(dta);
  }

  if (client.connected()) {
    client.stop();  // DISCONNECT FROM THE SERVER
  }
}

