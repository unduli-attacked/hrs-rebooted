/*
    TODO convert into importable library?
    
    This sketch sends data via HTTP GET requests to website and allows it to talk to the hardware systems
    THIS SKETCH SHOULD BE COPIED INTO THE FOLDER OF EVERY SKETCH THAT REQUIRES IT
    DO NOT EDIT allConnections.ino FILES DIRECTLY IN SKETCH FOLDERS

*/
//mainly pointless neo code //TODO if it's pointless why is it here?
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN            6
#define NUMPIXELS      16
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//more important
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

const char* host = "hrs.heliohost.org"; //TODO update
String sHost = "hrs.heliohost.org"; //TODO why are there 2. it's the same thing. why


String line;
String data;
String hosCode;
String rmNum;
int color = 0xFFFFFF; //TODO convert to run 100% in decimal/binary?
bool autoColor = true;
bool autoBright = true;
int age = 25;
int temp = 30;
int bright = 255;
bool aTemp = true;
bool aAutoBright = true;
bool aAutoColor = true;
float timezone = -420; //TODO daylight savings time support aka reason #3980 that I want to die


void setup() {
  Serial.begin(9600);

  // We start by connecting to a WiFi network

  WiFiManager wifiManager;
  wifiManager.autoConnect("testAP");
  Serial.println("Connected \n");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //fetches the raw room and hospital assignments from the server
  fetch(); //TODO why isn't this called IN assign()?
  //decodes the room and hospital assignments so the ESP can read them
  assign();

}



void loop() {

  //gets data from files on the server
  //TODO check that these still match up w/ files on the server, test, then delete loop to prevent redeclaration errors
  color = grabInt("color");
  autoColor = grabBool("autoColor");
  age = grabInt("age");
  autoBright = grabBool("autoBright");
  bright = grabInt("brightness");
  temp = grabInt("temp");
  autoAudio = grabBool("autoAud");
  autoVideo = grabBool("autoVideo");
  tzMins = grabInt("timeZone");
  aud = grabBool("audio");
  vid = grabBool("video");

  aAutoColor = grabAdmin("autoColor");
  aAutoBright = grabAdmin("autoBright");
  aTemp = grabAdmin("temp");

  timezone = grabFloat("timezone");

  //displays data
  Serial.println(color);
  Serial.println(autoColor);
  Serial.println(age);
  Serial.println(autoBright);
  Serial.println(bright);
  Serial.println(temp);

  //sends data to files on the server
  /*sennd("color", "2F2F2F");
    sennd("brightness", "26");
    sennd("temp", "30");
    delay(10000);*/
}

bool grabAdmin(String file) {
  String toFetch = file + "Admin"; //TODO wh- why would I do it this way? change WEBSITE FILES to special admin folder & add to path
  dataFetch(toFetch);
  if (data == "0") {
    return false;
  } else {
    return true;
  }
}





bool grabBool(String file) {
  dataFetch(file);
  if (data == "off") {
    return false;
  } else {
    return true;
  }
}

float grabFloat(String file) {
  dataFetch(file);
  float ret = data.toFloat();
  return ret;
}

int grabInt(String file) {
  dataFetch(file);
  //TODO wild fuckin plan here.... but maybe recreate the grabColor function and put this shit there?
  if (file == "color") {
    int coLen = data.length() + 1;
    char coA[coLen];
    data.toCharArray(coA, coLen);
    int i = 1;
    int p1;
    int p2;
    int red;
    int green;
    int blue;
    //TODO there's no way this is actually how I should be doing this. make it less dumb
    while (i <= 5) {
      int x = i + 1;
      if (coA[i] == '0' or coA[i] == '1' or coA[i] == '2' or coA[i] == '3' or coA[i] == '4' or coA[i] == '5' or coA[i] == '6' or coA[i] == '7' or coA[i] == '8' or coA[i] == '9') {
        p1 = coA[i] - '0';
      } else if (coA[i] == 'A' or coA[i] == 'B' or coA[i] == 'C' or coA[i] == 'D' or coA[i] == 'E' or coA[i] == 'F') {
        p1 = coA[i] - 'A';
        p1 = p1 + 10;
      } else if (coA[i] == 'a' or coA[i] == 'b' or coA[i] == 'c' or coA[i] == 'd' or coA[i] == 'e' or coA[i] == 'f') {
        p1 = coA[i] - 'a';
        p1 = p1 + 10;
      }
      p1 = p1 * 16;

      if (coA[x] == '0' or coA[x] == '1' or coA[x] == '2' or coA[x] == '3' or coA[x] == '4' or coA[x] == '5' or coA[x] == '6' or coA[x] == '7' or coA[x] == '8' or coA[x] == '9') {
        p2 = coA[x] - '0';
      } else if (coA[x] == 'A' or coA[x] == 'B' or coA[x] == 'C' or coA[x] == 'D' or coA[x] == 'E' or coA[x] == 'F') {
        p2 = coA[x] - 'A';
        p2 = p2 + 10;
      } else if (coA[x] == 'a' or coA[x] == 'b' or coA[x] == 'c' or coA[x] == 'd' or coA[x] == 'e' or coA[x] == 'f') {
        p2 = coA[x] - 'a';
        p2 = p2 + 10;
      }
      if (i == 1) {
        red = p1 + p2;
      } else if (i == 3) {
        green = p1 + p2;
      } else {
        blue = p1 + p2;
      }
      i = i + 2;
    }
    return pixels.Color(red, green, blue);
  } else {
    int tem = data.toInt();
    return tem;
  }
}




void sennd(String des, String toSend) { //TODO change name to sendData or something less stupid
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


  String dta = "h=" + hosCode + "&r=" + rmNum + "d8a=" + toSend + "&ds=" + des; //TODO wow it's like I was trying to confuse myself; fix var names
  //TODO figure out wtf this does exactly
  if (client.connect(sHost, 80)) { // REPLACE WITH YOUR SERVER ADDRESS
    Serial.println("Sending........\n\n");
    client.println("POST /" + hosCode + "/" + rmNum + "/add.php HTTP/1.1");
    Serial.println("POST /" + hosCode + "/" + rmNum + "/add.php HTTP/1.1");
    client.println("Host: " + sHost); // SERVER ADDRESS HERE TOO
    Serial.println("Host: " + sHost); // SERVER ADDRESS HERE TOO
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

void assign() {
  int len = line.length() + 1;
  char asinmt[len]; //TODO 10/10 naming conventions
  line.toCharArray(asinmt, len);
  String hos1 = String(asinmt[14]);
  String hos2 = String(asinmt[15]);
  String hos3 = String(asinmt[16]);
  String hos4 = String(asinmt[17]);
  String rm1 = String(asinmt[27]);
  rmNum = rm1;
  if (isDigit(asinmt[28])) { //TODO oh my fucking god WHY; fix this monstrosity
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


void dataFetch(String file) { //TODO change to return string line //TODO change name to fetch()
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
  url += "/";
  url += file;

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
    data = client.readStringUntil('\r');
    //Serial.println(data);
  }

  Serial.println();
  Serial.println("closing connection");
}

void fetch() { //TODO change to return string line //TODO change name to rep. what it actually does
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

