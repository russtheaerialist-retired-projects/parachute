#include "Bounce2.h"
#include "Servo.h"
#include <avr/sleep.h>


#define SERVO_PIN 9
#define READY_PIN 13
#define ARM_PIN 12
#define APOGEE_PIN 11
#define VERTICAL_PIN A0

#define SERVO_READY 0
#define SERVO_DEPLOY 180
#define ACCELERATION_THRESHOLD 150

Servo parachute;
Bounce arm;
bool armed = false;
bool button_state;
bool triggered = false;
int value;

void setup()
{
  Serial.begin(9600);
  pinMode(ARM_PIN, INPUT);
  digitalWrite(ARM_PIN, HIGH);  // Enable pull up resistor
  
  
  
  parachute.attach(SERVO_PIN);
  parachute.write(SERVO_READY);
  
  arm.attach(ARM_PIN);
  arm.interval(5);
}

void loop_armed()
{
  // get current acceleration
  // value = analogRead(VERTICAL_PIN);
  
  if (!triggered) {
    if (value > ACCELERATION_THRESHOLD) {
      triggered = true;
    }
  }
  
  if (triggered) {
    value = digitalRead(APOGEE_PIN);
    if (value == HIGH) {
      // deploy parachute
      
      delay(1000);
      noInterrupts();
      sleep_enable();
      sleep_mode();
    }
  }
  
}

void loop_unarmed()
{
  digitalWrite(READY_PIN, HIGH);
  delay(500);
  digitalWrite(READY_PIN, LOW);
  delay(500);
}

void loop()
{
  arm.update();
  button_state = arm.read() == LOW;
  Serial.print(button_state);
  Serial.print(" => ");
  Serial.println(armed);
  
  if (button_state != armed) {
    if (button_state) {
      // Transition to armed state
      digitalWrite(READY_PIN, HIGH);
    } else {
      // Transition to unarmed state
      parachute.write(SERVO_READY);
    }   
    armed = button_state;
  }
  
  armed ? loop_armed() : loop_unarmed();
}
