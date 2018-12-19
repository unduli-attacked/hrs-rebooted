// This is pointless AF and probably won't actually be used in this project but also it's cool as hell
// and it's possible it could somehow be used to control lights w/o built in drivers

int gLed = 8;
int rLed = 9;
int bLed = 10;
int full = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(gLed, OUTPUT);
  pinMode(rLed, OUTPUT);
  pinMode(bLed, OUTPUT);

  digitalWrite(gLed, LOW);
  digitalWrite(rLed, LOW);
  digitalWrite(bLed, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  colorMix(255, 242, 0, 10000);
}

void colorMix(int r, int g, int b, float ti) {
  int gPer = (g*100)/255;
  gPer = gPer*full;
  int rPer = (r*100)/255;
  rPer = rPer*full;
  int bPer = (b*100)/255;
  bPer = bPer*full;

  dim(rPer/100, gPer/100, bPer/100, ti);
}

void dim(float rus, float gus, float bus, float tim) {
  float gds = full - gus;
  float rds = full - rus;
  float bds = full - bus;
  //-us is up % of the duty cycle (out of 10)
  //-ds is down % of the duty cycle (out of 10)
  float startTime = millis();
  float lastTime = millis();
  while (startTime - lastTime <= tim) {
    if (rus == 0) {
      digitalWrite(rLed, LOW);
    } else {
      digitalWrite(rLed, HIGH);
      delay(rus);
      digitalWrite(rLed, LOW);
      delay(rds);
    }

    if (gus == 0) {
      digitalWrite(gLed, LOW);
    } else {
      digitalWrite(gLed, HIGH);
      delay(gus);
      digitalWrite(gLed, LOW);
      delay(gds);
    }

    if (bus == 0) {
      digitalWrite(bLed, LOW);
    } else {
      digitalWrite(bLed, HIGH);
      delay(bus);
      digitalWrite(bLed, LOW);
      delay(bds);
    }
    lastTime = millis();
  }

}
