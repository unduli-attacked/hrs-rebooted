
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  analogWrite(11, 0);
  Serial.println("initialized");


}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(11, 115);
  Serial.println("dance");
  delay(20000);
  analogWrite(11, 0);
  Serial.println("and freeze");
  delay(1000);
}
