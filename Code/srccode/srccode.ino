#define trigPin1 4
#define echoPin1 5
#define trigPin2 6
#define echoPin2 7
#define trigPin3 3
#define echoPin3 2
#include <SoftwareSerial.h>
#include <Wire.h>
#define emergency 3
#define compass 2
#define SENSOR 1
#define MINDISTANCE 50
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
SoftwareSerial bluetooth(0, 1);
long duration, distance, RightSensor, BackSensor, FrontSensor, LeftSensor;
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
int state = 1; int flag = 0;
int compassFlag = 0;
bool comp = true;
void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  delay(500);
}
void setup()
{
  Serial.begin (9600);
  bluetooth.begin(9600);
  

  /* Initialise the sensor */
  if (!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1);
  }
  Serial.println("abccc");
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, INPUT);
}

void loop() {
  int input = digitalRead(10);
  if (input == 1) {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    if (flag == 0) {
      flag = 1;
    }
    delay(6000);
    input = digitalRead(10);
    if (input == 1) {
      flag = 2;
    }
  }
  if (flag == 1) {
    state = compass;
    flag = 0;
  }
  if (flag == 2) {
    flag = 0;
    state = emergency;
  }
  if (state == compass) {
    if (comp) {
      Serial.println("3");
      comp = false;
    }
    operateCompass();
  }
  else if (state == emergency) {
    operateEmergency();
  }
  else {
    operateSensor();
  }
}
void operateSensor(void) {
  SonarSensor(trigPin1, echoPin1);
  RightSensor = distance;
  SonarSensor(trigPin2, echoPin2);
  LeftSensor = distance;
  SonarSensor(trigPin3, echoPin3);
  FrontSensor = distance;
  if (FrontSensor < MINDISTANCE) {
    if (RightSensor < MINDISTANCE && LeftSensor < MINDISTANCE) {
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      Serial.println("6");
    }
    else {
      if (RightSensor > LeftSensor) {
        digitalWrite(9, HIGH);
        digitalWrite(8, LOW);
        Serial.println("2");
        Serial.print("Distance ");
        Serial.print(RightSensor);
        Serial.println(" centimeter");
      }
      else {
        digitalWrite(8, HIGH);
        digitalWrite(9, LOW);
        Serial.println("1");
        Serial.print("Distance ");
        Serial.print(LeftSensor);
        Serial.println(" centimeter");
      }
    }
  }
  else {
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
  }
//    Serial.print(LeftSensor);
//    Serial.print(" - ");
//    Serial.print(FrontSensor);
//    Serial.print(" - ");
//    Serial.println(RightSensor);
  delay(5000);
}


void operateCompass(void) {

  sensors_event_t event;
  mag.getEvent(&event);

   
  float heading = atan2(event.magnetic.y+31, event.magnetic.x-8);
   
  if (heading < 0)
    heading += 2 * PI;
  if (heading > 2 * PI)
    heading -= 2 * PI;
  float headingDegrees = heading * 180 / M_PI;
//   Serial.print(headingDegrees);
//   Serial.println("hi");
  if (headingDegrees > 340 || headingDegrees < 20)
  { Serial.println("4");
    compassFlag++;
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    delay(500);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
  }
  if (compassFlag == 3) {
    compassFlag = 0;
    state = SENSOR;
    comp = true;
  }
  delay(500);
}
void operateEmergency(void) {
  Serial.println("5");
  state = SENSOR;
  delay(10000);
}
void SonarSensor(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  delay(50);
}
