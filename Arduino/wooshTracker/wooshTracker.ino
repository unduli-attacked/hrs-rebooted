int ir = A3;
int a;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ir, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  a = analogRead(ir);
  Serial.println(a);
  delay(500);
}
