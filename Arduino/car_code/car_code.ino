#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define PIXEL_COUNT 60
#define PIXEL_PIN 11

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

//Directional commands
#define GO_FORWARD 8
#define GO_RIGHT 6
#define GO_LEFT 4
#define GO_BACK 2

int pinLB = A1;   // Define left motor IN4 Pin A0
int pinLF = A0;   // Define left motor IN3 Pin A1

int pinRB = A3;  // Define right motor IN2 Pin A2
int pinRF = A2;  // Define right motor IN1 Pin A3


int pinLENA = 5; //Define left motor ENABLE Pin 5 (PWM)
int pinRENA = 6; //Define right motor ENABLE Pin 6 (PWM)

int trigPin = 8; //Define the ultrasound signal emission Pin TRIG
int echoPin = 9;  // Define the ultrasound signal receiving a Pin ECHO

//int LbumperPin = 4;
//int RbumperPin = 7;

int servoPin = 11; // Servo signal pin (orange)

int turnSpeedL = 180;
int turnSpeedR = 180;
int forwardSpeed = 245;
int backSpeed = 70;

int a = 15;
float mspd = 3.89;

int direction = 0;   // After the former = 8 = 2 left = 4 right = 6

Servo myservo;        // Set up the myservo

int delay_time = 500; // After the servo motor to the stability of the time


float Fdistance = 0;
float Ldistance = 0;
float Rdistance = 0;


void setup()
{
  Serial.begin(9600);     // Initialize
  pinMode(pinLB, OUTPUT); // Define 14 pin for the output (PWM)
  pinMode(pinLF, OUTPUT); // Define 15 pin for the output (PWM)
  pinMode(pinRB, OUTPUT); // Define 16 pin for the output (PWM)
  pinMode(pinRF, OUTPUT); // Define 17 pin for the output (PWM)

  // pinMode(ora, OUTPUT);
  //pinMode(whi, OUTPUT);

  pinMode(pinLENA,  OUTPUT);  // Define 5 pin for PWM output
  pinMode(pinRENA,  OUTPUT);  // Define 6 pin for PWM output

  pinMode(echoPin, INPUT);    // Define the ultrasound enter pin
  pinMode(trigPin, OUTPUT);  // Define the ultrasound output pin

  //  pinMode(LbumperPin, INPUT);    // Define the bumper pin
  //  pinMode(RbumperPin, INPUT);    // Define the bumper pin

  myservo.attach(servoPin);    // Define the servo motor output 10 pin(PWM)
  direction = GO_FORWARD;

  strip.begin();
  strip.show();

  Serial.println("Hello! My name is Puck!");
  Serial.println("Please make sure my sensors are attached so I don't run into anything!");
}

void loop()
{
  //myservo.write(90);  /*Make the servo motor ready position Prepare the next measurement */
  //detection();        //Measuring Angle And determine which direction to go to
  goodScanning();

  if (direction == GO_BACK)
  {
    back(500);                    //  back
    //turnL(100);                   //Move slightly to the left (to prevent stuck in dead end lane)
    Serial.print(" Reverse ");   //According to the direction (reverse)
    colorWipe(0xFF0000, 80);
    delay(10);
    colorWipe(0x000000, 80);
  }
  if (direction == GO_RIGHT)
  {
    //back(100);
    turnR(350);                   // right
    Serial.print(" Right ");    //According to the direction (Right)
    //digitalWrite(ora, LOW);
  }
  if (direction == GO_LEFT)
  {
    //back(100);
    turnL(350);                  // left
    Serial.print(" Left ");     //According to the direction (Left)
    //digitalWrite(ora, LOW);
  }
  if (direction == GO_FORWARD)
  {
    advance();                 // go
    Serial.print(" Advance ");   //According to the direction (Advance)
    Serial.print("   ");
    //digitalWrite(ora, LOW);
  }
  Serial.println();
}

void advance()     // go
{
  digitalWrite(pinRB, HIGH); // 16 feet for high level
  digitalWrite(pinRF, LOW);  //17 feet for low level
  analogWrite(pinRENA, forwardSpeed); //Set the output speed(PWM)
  digitalWrite(pinLB, HIGH); // 14 feet for high level
  digitalWrite(pinLF, LOW);  //15 feet for high level
  analogWrite(pinLENA, forwardSpeed); //Set the output speed(PWM)
}

void advance(int a)     // go
{
  advance();
  delay(a);
  stopp();
}

void turnR(int d)        //right
{
  digitalWrite(pinRB, HIGH);
  digitalWrite(pinRF, LOW);
  analogWrite(pinRENA, turnSpeedR);
  digitalWrite(pinLB, LOW);
  digitalWrite(pinLF, HIGH);
  analogWrite(pinLENA, turnSpeedR);
  delay(d);
  stopp();
}
void turnL(int e)        //left
{
  digitalWrite(pinRB, LOW);
  digitalWrite(pinRF, HIGH);
  analogWrite(pinRENA, turnSpeedL);
  digitalWrite(pinLB, HIGH);
  digitalWrite(pinLF, LOW);
  analogWrite(pinLENA, turnSpeedL);
  delay(e);
  stopp();
}

void stopp()         //stop
{
  digitalWrite(pinRB, LOW);
  digitalWrite(pinRF, LOW);
  digitalWrite(pinLB, LOW);
  digitalWrite(pinLF, LOW);
}

void back(int g)          //back
{
  digitalWrite(pinRB, LOW);
  digitalWrite(pinRF, HIGH);
  analogWrite(pinRENA, backSpeed);
  digitalWrite(pinLB, LOW);
  digitalWrite(pinLF, HIGH);
  analogWrite(pinLENA, backSpeed);
  delay(g);
  stopp();
}

void goodScanning() {
  int d;
  if (a > 165) {
    a = 15;
  }
  myservo.write(a);
  delay(delay_time);
  derp(d);
  myservo.write(90);
  delay(delay_time);
  derp(d);
  a = a +90;
  myservo.write(a);
  delay(delay_time);
  derp(d);
  a = a -90;
  a++;
  
}

void derp(int d){
  d = getDistance();
  Serial.println(d);
  if(d < 30){
    stopp();
    boop();
  }
}

void boop(){
  if (15 <= a < 90){
    //right
    turnR(a*mspd);
  }else if (90 < a <= 165){
    //left
    turnL(a*mspd);
  }
  back(400);
}

void detection()        //Measuring three angles(0.90.179)
{
  check_front();            // Read in front of the distance

  //  if (digitalRead(RbumperPin) == LOW){
  //    back(500);
  //    turnL(350);
  //  }
  //  else if (digitalRead(LbumperPin) == LOW){
  //    back(500);
  //    turnR(350);
  //  }

  if (Fdistance < 15)        // If the front distance less than 10 cm
  {
    back(200);                // The back two milliseconds
  }

  if (Fdistance < 30)        // If the front distance less than 25 cm
  {
    stopp();               // Remove the output data
    check_left();            // Read the left distance

    myservo.write(90);
    delay(delay_time);      // Waiting for the servo motor is stable

    check_right();            // Read the right distance

    if (Ldistance < 15 && Rdistance < 15)   /*If the left front distance and distance and the right distance is less than 15 cm */
    {
      direction = GO_BACK;      //Walk backwards
    }
    else if (Ldistance > Rdistance)  //If the distance is greater than the right distance on the left
    {
      direction = GO_LEFT;      //Left
    }
    else if (Ldistance <= Rdistance)  //If the distance is less than or equal to the distance on the right
    {
      direction = GO_RIGHT;      //right
    }

  }
  else                      //If the front is not less than 25 cm (greater than)
  {
    direction = GO_FORWARD;        //Walk forward
  }

  myservo.write(90);
  //delay(delay_time);
}
void check_front()   // Measure the distance ahead
{
  myservo.write(90);
  if (direction != GO_FORWARD) {
    delay(delay_time);
  }
  else {
    // delay(10);
  }
  Fdistance = getDistance();  // A time to distance distance (unit: cm£©
  Serial.print("F distance:");      //The output distance (unit: cm)
  Serial.println(Fdistance);         //According to the distance

}

void check_left()   // Measure the distance on the left
{
  myservo.write(167);
  delay(delay_time);

  Ldistance = getDistance();  // Will be time to distance distance (unit: cm)
  Serial.print("L distance:");       //The output distance (unit: cm)
  Serial.println(Ldistance);         //According to the distance
}
void check_right()   // Measure the distance on the right
{
  myservo.write(15);
  delay(delay_time);

  Rdistance = getDistance();  //Will be time to distance distance (unit: cm)
  Serial.print("R distance:");       //The output distance (unit: cm)
  Serial.println(Rdistance);         //According to the distance
}

float getDistance() {
  digitalWrite(trigPin, LOW);   // For low voltage 2 us ultrasonic launch
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // Let ultrasonic launch 10 us high voltage, there is at least 10 us
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);    // Maintaining low voltage ultrasonic launch
  float distance = pulseIn(echoPin, HIGH);  // Read the time difference
  distance = distance / 5.8 / 10;  //Will be time to distance distance (unit: cm)
  return distance;
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
