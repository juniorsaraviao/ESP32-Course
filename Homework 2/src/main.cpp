#include <Arduino.h>

// Objective 1
#define echo 14
#define trigger 12
#define ledGreen 32
#define ledRed 26
#define ultrasonicDivisor 58
#define minDistance 30
#define blinkTime 100 // blink delay
uint8_t distance = 0;
volatile bool change = false;
unsigned long t = 0;

hw_timer_t  *timer_1  = NULL;

// Objective 2
const int ledPin = 16;
const int freq = 50;
const int ledChannel = 0;
const int resolution = 12;
const int sensor = 13;

hw_timer_t  *timer_2  = NULL;

void IRAM_ATTR onTimer(){
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  t = pulseIn(echo, HIGH);
  distance = t/ultrasonicDivisor;

  if(distance > minDistance){
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);

  }else{
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledRed, HIGH);    
    delayMicroseconds(blinkTime);
    digitalWrite(ledRed, LOW);
  }
}

void IRAM_ATTR onTimer2(){
  int output = digitalRead(sensor);
  Serial.println(output);
  if (output==1) {           
    ledcWrite(ledChannel, 20);
    delayMicroseconds(10);
    ledcWrite(ledChannel, 500);
    delayMicroseconds(10);
    ledcWrite(ledChannel, 820);
    delayMicroseconds(10);
  }
  delayMicroseconds(10);
}

void setup() { 
  Serial.begin(9600);
  // Objective 1
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

  timer_1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer_1, &onTimer, true);
  timerAlarmWrite(timer_1, 100000, true); // 100 miliseconds
  timerAlarmEnable(timer_1);

  // Objective 2
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  ledcWrite(ledChannel, 0);

  timer_2 = timerBegin(0, 90, true);
  timerAttachInterrupt(timer_2, &onTimer2, true);
  timerAlarmWrite(timer_2, 200000, true); // 200 miliseconds
  timerAlarmEnable(timer_2);

  pinMode(sensor,INPUT);
}

void loop() {    
  
}