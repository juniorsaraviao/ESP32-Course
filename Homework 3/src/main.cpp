#include <Arduino.h>
#include "xht11.h"
#include "lcd128_32_io.h"

#define IN1 2 
#define IN2 15 
#define ENA 0 //Enable del l293d
#define pin_pot 4

const int freq = 1000;
const int resolution = 12;
const int motorChannelRight = 0;
const int motorChannelLeft = 1;
int motorVel;

char direction;

hw_timer_t *timer_pot  = NULL;

void IRAM_ATTR onTimer1(){ 
  motorVel = analogRead(pin_pot);
  if (direction == 'D' || direction == 'd'){
      ledcWrite(motorChannelRight, motorVel);
      ledcWrite(motorChannelLeft, 0);
  }
  if (direction == 'I' || direction == 'i'){
      ledcWrite(motorChannelRight, 0);
      ledcWrite(motorChannelLeft, motorVel);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(IN1,OUTPUT); 
  pinMode(IN2,OUTPUT); 
  pinMode(ENA,OUTPUT); 
  pinMode(pin_pot,INPUT);
  digitalWrite(ENA, HIGH);

  ledcSetup(motorChannelRight, freq, resolution);
  ledcAttachPin(IN1, motorChannelRight);
  ledcWrite(motorChannelRight, 0);

  ledcSetup(motorChannelLeft, freq, resolution);
  ledcAttachPin(IN2, motorChannelLeft);
  ledcWrite(motorChannelLeft, 0); 

  timer_pot = timerBegin(0, 80, true);
  timerAlarmWrite(timer_pot, 100000, true);
  timerAttachInterrupt(timer_pot, &onTimer1, true);
  timerAlarmEnable(timer_pot);
}

void loop() {
  if (Serial.available() > 0) {
    Serial.print("Key: ");
    direction = Serial.read();
    Serial.println(direction);
  }
}