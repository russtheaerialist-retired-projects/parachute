#include <Servo.h>
#include <avr/sleep.h>
#include <Bounce2.h>

///////////////////////////////////////////////////////
// Configuration Section
//////////////////////////////////////////////////////

////////// Options

// Put device into sleep mode to reduce battery consumption after triggering.
#define POWER_DOWN_AFTER_TRIGGER

// Should the arm pin use the internal pullup
#define USE_INTERNAL_PULLUP

// Is arm LOW or HIGH on the switch
#define TIGGER_VALUE LOW

////////// Pin Configuration

// Trigger Servo Pin
#define SERVO_PIN 9

// Ready LED (13 is the onboard led)
#define READY_PIN 13

// Switch, pulled up high
#define ARM_PIN 2

// 0G Logic pin off of the Accelerometer, used to detect free fall
#define APOGEE_PIN 11

// Analog pin for the Z-axis of the accelerometer
#define VERTICAL_PIN A0

////////// Tuning Points

// Degree for the armed and ready position
#define SERVO_READY 0

// Degree for the deployed position to unhook the rubberband
#define SERVO_DEPLOY 180

// Don't trigger until we've hit at least this vertical acceleration
#define ACCELERATION_THRESHOLD 600

/////////////////////////////////////////////////////////

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
#ifdef USE_INTERNAL_PULLUP
  digitalWrite(ARM_PIN, HIGH);  // Enable pull up resistor
#endif // USE_INTERNAL_PULLUP
  
  // Accel data for Z axis, ignore all others.
  pinMode(VERTICAL_PIN, INPUT);
  pinMode(APOGEE_PIN, INPUT);
  
  parachute.attach(SERVO_PIN);
  parachute.write(SERVO_READY);
  
  arm.attach(ARM_PIN);
  arm.interval(5);
  
  
}

#ifdef POWER_DOWN_AFTER_TRIGGER
   void pinInterrupt()
   {
     detachInterrupt(0);
   }
   
   inline void sleep()
   {
      attachInterrupt(0, pinInterrupt, HIGH);
      delay(100);

      // Enable us to power down after parachute deployment to save battery power.
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      sleep_mode();
      sleep_disable();

      // Reset the state
      triggered = false;
      parachute.write(SERVO_READY);
   }
#else // POWER_DOWN_AFTER_TRIGGER
   inline void sleep() {}
#endif // POWER_DOWN_AFTER_TRIGGER

void loop_armed()
{
  // get current acceleration
  value = analogRead(VERTICAL_PIN);

  if (!triggered) {
    if (value > ACCELERATION_THRESHOLD) {
      Serial.println("Launch detected");
      triggered = true;
    }
  }
  
  if (triggered) {
    value = digitalRead(APOGEE_PIN);
    Serial.println(value);

    if (value == HIGH) {
      parachute.write(SERVO_DEPLOY);
      delay(1000);

      digitalWrite(READY_PIN, LOW);
      sleep();
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
  button_state = arm.read() == TIGGER_VALUE;
  
  if (button_state != armed) {
    if (button_state) {
         Serial.println("Arming");
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
