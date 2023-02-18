#include <Arduino.h>

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
#define SOUND_SPEED 0.034

hw_timer_t  *timer_1  = NULL;

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

void setup() { 
  Serial.begin(9600);
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);

  timer_1 = timerBegin(0, 80, true);
  timerAttachInterrupt(timer_1, &onTimer, true);
  timerAlarmWrite(timer_1, 100000, true); // 100 miliseconds
  timerAlarmEnable(timer_1);
}

void loop() {    

}