/*
    This sketch sends data via HTTP GET requests to data.sparkfun.com service.

    You need to get streamId and privateKey at data.sparkfun.com and paste them
    below. Or just customize this script to talk to other HTTP servers.

*/
//mainly pointless neo code
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN            6
#define NUMPIXELS      16
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//more important
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

const char* host = "teamcosmo.5gbfree.com";

bool grabAdmin(String file) {
  String toFetch = file + "Admin";
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

int grabInt(String file) {
  dataFetch(file);
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
    return pixel.Color(red, green, blue);
  } else {
    int tem = data.toInt();
    return tem;
  }
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


void dataFetch(String file) {
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

