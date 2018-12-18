int minOfDay = 0;
float bright = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  daylight();
}
void daylight() {
  float a;
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
  if (minOfDay <= 1140 && minOfDay >= 390) {
    a = ((yr*-1) + yp) / ((xp * xp) - (xp * xs));
    b = (xs*-1) * (((yr*-1) + yp) / ((xp * xp) - (xp * xs)));
    c = yr;
    squared = true;
  } else if (minOfDay <= 390 && minOfDay >= 270) {
    d = (((2*xr)-(xr*yr))/120)+yr;
    m = (yr-2)/120;
    squared = false;
  } else if (minOfDay >= 1140 && minOfDay <= 1260) {
    d = (((xs*ys)-(2*xs))/120)+ys;
    m = (2-ys)/120;
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
    calc5 = calc4 + c;
  } else {
    float calc1 = m * minOfDay;
    calc5 = calc1 + d;
  }
  bright = calc5;
  if (minOfDay < 1440){
    Serial.print(bright);
    Serial.print(" , ");
    Serial.println(minOfDay);
    minOfDay++;
  }
}
