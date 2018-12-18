#include <Servo.h>

//Directional commands
#define GO_FORWARD 8
#define GO_RIGHT 6
#define GO_LEFT 4
#define GO_BACK 2

int pinLB = A0;   // Define left motor IN4 Pin A0
int pinLF = A1;   // Define left motor IN3 Pin A1

int pinRB = A2;  // Define right motor IN2 Pin A2
int pinRF = A3;  // Define right motor IN1 Pin A3

int pinLENA = 5; //Define left motor ENABLE Pin 5 (PWM)
int pinRENA = 6; //Define right motor ENABLE Pin 6 (PWM)

int trigPin = 8; //Define the ultrasound signal emission Pin TRIG
int echoPin = 9;  // Define the ultrasound signal receiving a Pin ECHO

//int LbumperPin = 4;
//int RbumperPin = 7;

int servoPin = 11; // Servo signal pin (orange)

int turnSpeed = 250;
int forwardSpeed = 100;
int backSpeed = 100;

int direction = 0;   // After the former = 8 = 2 left = 4 right = 6

Servo myservo;        // Set up the myservo

int delay_time = 500; // After the servo motor to the stability of the time


float Fdistance = 0;
float Ldistance = 0;
float Rdistance = 0;

unsigned long lastTime;
double input, setpoint, output;
double iErr, lastErr;
double kp, ki, kd;

int winSize = 5000;
unsigned long winStart;

void setup()
{
  Serial.begin(9600);     // Initialize
  pinMode(pinLB, OUTPUT); // Define 14 pin for the output (PWM)
  pinMode(pinLF, OUTPUT); // Define 15 pin for the output (PWM)
  pinMode(pinRB, OUTPUT); // Define 16 pin for the output (PWM)
  pinMode(pinRF, OUTPUT); // Define 17 pin for the output (PWM)

  pinMode(pinLENA,  OUTPUT);  // Define 5 pin for PWM output
  pinMode(pinRENA,  OUTPUT);  // Define 6 pin for PWM output

  pinMode(echoPin, INPUT);    // Define the ultrasound enter pin
  pinMode(trigPin, OUTPUT);  // Define the ultrasound output pin

  //  pinMode(LbumperPin, INPUT);    // Define the bumper pin
  //  pinMode(RbumperPin, INPUT);    // Define the bumper pin

  myservo.attach(servoPin);    // Define the servo motor output 10 pin(PWM)
  winStart = millis();
}

void loop()
{
  //stopp();
  setTuning(1,.0001,1);
  input = getDistance();
  setpoint = 0;
  compute();
  /*if(output<100){
    output++;
  }else{
    output--;
  }*/

  Serial.print(input);
  Serial.print(",");
  Serial.println(output);

  analogWrite(pinLENA, output);
  analogWrite(pinRENA, output);

  /*if(output<0){
    backk();
  }else{
    advance();
  }*/
  backk();
}
void compute () {
  unsigned long now = millis();
  double timeChange = (double)(now - lastTime);
  double pErr = -1*(setpoint - input);
  iErr += pErr * timeChange;
  double dErr = (pErr - lastErr) / timeChange;

  output = kp * pErr;//r + ki * iErr;// + kd * dErr;

  lastTime = now;
  lastErr = pErr;
}
void setTuning(double Kp, double Ki, double Kd) {
  kp = Kp;
  ki = Ki;
  kd = Kd;
}

void advance()     // go
{
  digitalWrite(pinRB, HIGH); // 16 feet for high level
  digitalWrite(pinRF, LOW);  //17 feet for low level
  // analogWrite(pinRENA, forwardSpeed); //Set the output speed(PWM)
  digitalWrite(pinLB, HIGH); // 14 feet for high level
  digitalWrite(pinLF, LOW);  //15 feet for high level
  // analogWrite(pinLENA, forwardSpeed); //Set the output speed(PWM)
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
  // analogWrite(pinRENA, turnSpeed);
  digitalWrite(pinLB, LOW);
  digitalWrite(pinLF, HIGH);
  // analogWrite(pinLENA, turnSpeed);
  delay(d);
  stopp();
}
void turnL(int e)        //left
{
  digitalWrite(pinRB, LOW);
  digitalWrite(pinRF, HIGH);
  // analogWrite(pinRENA, turnSpeed);
  digitalWrite(pinLB, HIGH);
  digitalWrite(pinLF, LOW);
  // analogWrite(pinLENA, turnSpeed);
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
  // analogWrite(pinRENA, backSpeed);
  digitalWrite(pinLB, LOW);
  digitalWrite(pinLF, HIGH);
  // analogWrite(pinLENA, backSpeed);
  delay(g);
  stopp();
}

void backk(){
  digitalWrite(pinRB, LOW);
  digitalWrite(pinRF, HIGH);
  digitalWrite(pinLB, LOW);
  digitalWrite(pinLF, HIGH);
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
