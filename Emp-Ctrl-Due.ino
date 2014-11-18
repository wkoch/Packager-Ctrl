/*
  Emp-Ctrl-Due - v2.0a - 14/11/2014
  Arduino Due application for packaging machine automation.
  Created by William Koch.
  Released into the public domain.
*/

#include <Modus.h>
#include <Debounce.h>
#include <OnePush.h>
#include <Cyclic.h>
#include <SlowPWM.h>
#include "Config.h"

// Operation Modes
#define WARMUP      0 // Warm-up, a few seconds of delay.
#define STANDBY     1 // Some buttons are working.
#define STARTING    2 // 3 empty cycles.
#define PRODUCTION  3 //
#define STOPPING    4 // Finishs current cycle, empties the machine and stops.
#define COOLDOWN    5 //
#define MAINTENANCE 6 // Maintenance mode.
#define ALARM       7 // Alarm mode.

#define IN     INPUT_PULLUP  // INPUT or INPUT_PULLUP
#define BUTTON INPUT_PULLUP  // INPUT or INPUT_PULLUP
#define SENSOR INPUT_PULLUP  // INPUT or INPUT_PULLUP
#define OUT    OUTPUT
#define ON     HIGH
#define OFF    LOW
#define BETWEEN(x, a, b)  ((a) <= (x) && (x) <= (b))

Modus modes(7);
Cyclic cycle(2000, 900);

void setup() {
  // BUTTONS
  pinMode(general.in, BUTTON);
  pinMode(dater.in, BUTTON);
  pinMode(feeder.in, BUTTON);
  // SENSORS
  pinMode(sensor.reset, SENSOR);
  pinMode(sensor.photocell, SENSOR);
  pinMode(sensor.security1, SENSOR);
  pinMode(sensor.security2, SENSOR);
  pinMode(sensor.security3, SENSOR);

  // OTHER
  pinMode(vWelder.out, OUT);
  pinMode(hWelder.out, OUT);
  pinMode(dWelder.out, OUT);
  pinMode(general.out, OUT);
  pinMode(feeder.out, OUT);
  pinMode(dater.out, OUT);
  pinMode(dater.led, OUT);
  pinMode(jaw.out, OUT);
  pinMode(photocell.out, OUT);
  pinMode(knife.out, OUT);
  pinMode(cooler.out, OUT);
  pinMode(welder.out, OUT);

  // Shut OFF at startup
  allOFF();
}

void loop() {
  security();
  if (modes.status(ALARM)) {
    cycle.stop();
    lockAll();
    allOFF();
    // Turn a led ON?
    // Turn everything else OFF
    // Put Arduino to sleep.
  } else if (modes.status(WARMUP)) {
    lockAll();
    allOFF();
    delay(3000);
    modes.set(STANDBY);
  } else if (modes.status(STANDBY)) {
    lockAll();
    allOFF();
    // Buttons work.
    digitalWrite(feeder.out, feeder.button.state());
    digitalWrite(general.out, general.button.state());
    if (digitalRead(general.out) == ON) { // If general is ON:
      unlockAll();
      modes.set(STARTING); // Change to Starting mode.
    }
  } else if (modes.status(STARTING)) {
    // 3 empty cycles.
    cycle.start(); // Starts the cycle clock, if not started yet.
    cycle.update(); // Updates the cycle clock.
    weldersPWM();
    if (digitalRead(feeder.out) == ON) { // If the feeder is ON:
      feeder.button.next(); // Simulate button press and.
      digitalWrite(feeder.out, feeder.button.state()); // Turn the feeder OFF.
    }
    digitalWrite(general.out, general.button.state()); // Turns general ON.
    if (digitalRead(general.out) == OFF) { // If general button is pressed.
      cycle.stop();
      modes.set(STANDBY); // Shuts down again and changes to Starting mode.
    }
    reset();
    if (cycle.cycles() >= 3) { // If 3 or more cycles were completed:
      modes.set(PRODUCTION); // Changes to Prodution mode.
    }
  } else if (modes.status(PRODUCTION)) {
    // Normal production.
    digitalWrite(feeder.out, feeder.button.state());
    digitalWrite(dater.led, dater.button.state());
    weldersPWM();
    Schedule(dater);
    Schedule(jaw);
    Schedule(photocell);
    Schedule(knife);
    Schedule(cooler);
    Schedule(welder);
    reset();
    if (cycle.cycles() > 4 && !general.button.status()) { // If general button is pressed.
      if (digitalRead(feeder.out) == ON) { // If the feeder is ON:
        feeder.button.next(); // Simulate button press and.
        digitalWrite(feeder.out, feeder.button.state()); // Turn the feeder OFF.
      }
      if (digitalRead(dater.out) == ON) { // If the dater is ON:
        dater.button.next(); // Simulate button press and.
        digitalWrite(dater.out, dater.button.state()); // Turn the dater OFF.
      }
      modes.set(STOPPING); // Begins soft shutdown.
    }
  } else if (modes.status(STOPPING)) {
    // Shutting down.
    weldersPWM();
    if (digitalRead(dater.out) == ON || !dater.lock) {
      dater.lock = true;
      digitalWrite(dater.out, OFF);
    }
    if (!feeder.lock) {
      feeder.lock = true;
      digitalWrite(dater.out, OFF);
    }
    Schedule(jaw);
    Schedule(photocell);
    Schedule(knife);
    Schedule(cooler);
    Schedule(welder);
    reset();
    if (cycle.now() > jaw.stop || cycle.now() == 0) {
      modes.set(COOLDOWN);
    }
  } else if (modes.status(COOLDOWN)) {
    cycle.stop();
    allOFF();
    digitalWrite(cooler.out, ON);
    delay(1000);
    digitalWrite(cooler.out, OFF);
    cycle.reboot(); // Restarts the cycle counter and clock.
    modes.set(STANDBY);
  } else if (modes.status(MAINTENANCE)) {
    // Manually control things.
    // Check everything individually.
    Serial.begin(9600); // TODO: Put inside a conditional
  }
}

// === CUTER FUNCTIONS THAN ARDUINO'S.
boolean isON(byte pin) {
  if (digitalRead(pin) == ON) {
    return true;
  } else {
    return false;
  }
}

boolean isOFF(byte pin) {
  if (digitalRead(pin) == OFF) {
    return true;
  } else {
    return false;
  }
}

void turnON(byte pin) {
  if (isOFF(pin)) {
    digitalWrite(pin, ON);
  }
}

void turnOFF(byte pin) {
  if (isON(pin)) {
    digitalWrite(pin, OFF);
  }
}
// ===

void reset() {
  if (isOFF(sensor.reset)) { // If reset sensor is pressed. (PULLUP)
    cycle.reset(); // Resets the cycle;
  }
}

void weldersPWM() {
  !vWelder.lock ? vWelder.pwm.on() : vWelder.pwm.off();
  !hWelder.lock ? hWelder.pwm.on() : hWelder.pwm.off();
  !dWelder.lock ? dWelder.pwm.on() : dWelder.pwm.off();
}

void lockAll() {
  vWelder.lock = hWelder.lock = dWelder.lock = general.lock = feeder.lock =
  dater.lock = jaw.lock = photocell.lock = knife.lock = cooler.lock =
  welder.lock = true;
}

void unlockAll() {
  vWelder.lock = hWelder.lock = dWelder.lock = general.lock = feeder.lock =
  dater.lock = jaw.lock = photocell.lock = knife.lock = cooler.lock =
  welder.lock = false;
}

void allOFF() {
  vWelder.pwm.off();
  hWelder.pwm.off();
  dWelder.pwm.off();
  digitalWrite(vWelder.out, OFF);
  digitalWrite(hWelder.out, OFF);
  digitalWrite(dWelder.out, OFF);
  if (modes.mode() != STANDBY) {
    digitalWrite(general.out, OFF);
    digitalWrite(feeder.out, OFF);
    digitalWrite(dater.out, OFF);
  }
  digitalWrite(dater.led, OFF);
  digitalWrite(jaw.out, OFF);
  digitalWrite(photocell.out, OFF);
  digitalWrite(knife.out, OFF);
  digitalWrite(cooler.out, OFF);
  digitalWrite(welder.out, OFF);
}

void updateAll() {
  cycle.update();
}

void Schedule(struct function f) {

  if (isON(dater.led)) {
    dater.lock = false;
  } else {
    dater.lock = true;
  }
  unsigned long start;
  f.start == 0 ? start = 0 : start = (f.start * cycle.last()) / 1500;
  unsigned long stop;
  f.stop == 0 ? stop = 0 : stop = (f.stop * cycle.last()) / 1500;
  if (f.lock == true) { // Locked function
    if (isON(f.out)) { digitalWrite(f.out, OFF); }
  } else { // Unlocked function.
    if (start < stop && BETWEEN(cycle.now(), start, stop)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      digitalWrite(f.out, ON);
    } else if (start > stop && !BETWEEN(cycle.now(), stop, start)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      digitalWrite(f.out, ON);
    } else { if (isON(f.out)) { digitalWrite(f.out, OFF); } }
  }
}

void security() {
  byte s1, s2, s3;
  s1 = isON(sensor.security1);
  s2 = isON(sensor.security2);
  s3 = isON(sensor.security3);
  if (!s1 || !s2 || !s3) { // Negated because of INPUT_PULLUP.
    if (modes.mode() != ALARM) {
      modes.set(ALARM);
    }
  }
}

void knifeSecurity() {
  if (digitalRead(sensor.jaw) == ON && digitalRead(jaw.out) == ON) {
    modes.set(ALARM);
  }
}
