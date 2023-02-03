#include <Arduino.h>

#define echo 14
#define trigger 12
#define ledGreen 32
#define ledRed 26
#define ultrasonicDivisor 58
#define minDistance 30
#define blinkTime 100 // 100 Hz = 10 miliseconds but it does not show blinking
uint8_t distance = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(9600);
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
}

void loop() {

  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  t = pulseIn(echo, HIGH);
  distance = t/ultrasonicDivisor;
  Serial.print("The distance is: ");
  Serial.println(distance);

  if(distance > minDistance){

    Serial.println("Turning on Green led / Turning off Red led");
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);

  }else{

    Serial.println("Turning on Red led / Turning off Green led");
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);    
    delay(blinkTime);
    digitalWrite(ledRed, LOW);
    delay(blinkTime);

  }

  delay(10);  

}