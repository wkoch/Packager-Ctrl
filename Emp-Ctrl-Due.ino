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
Cyclic cycle(5000, 900);

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
  digitalWrite(vWelder.out, OFF);
  digitalWrite(hWelder.out, OFF);
  digitalWrite(dWelder.out, OFF);
  digitalWrite(general.out, OFF);
  digitalWrite(feeder.out, OFF);
  digitalWrite(dater.out, OFF);
  digitalWrite(dater.led, OFF);
  digitalWrite(jaw.out, OFF);
  digitalWrite(photocell.out, OFF);
  digitalWrite(knife.out, OFF);
  digitalWrite(cooler.out, OFF);
  digitalWrite(welder.out, OFF);
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
    delay(3000);
    modes.set(STANDBY);
  } else if (modes.status(STANDBY)) {
    // Buttons work.
    /*general.button.update(); // Updates general button.
    feeder.button.update(); // Updates feeder button.*/
    digitalWrite(feeder.out, feeder.button.state());
    digitalWrite(general.out, general.button.state());
    if (digitalRead(general.out) == ON) { // If general is ON:
      modes.set(STARTING); // Change to Starting mode.
    }
  } else if (modes.status(STARTING)) {
    // 3 empty cycles.
    cycle.start(); // Starts the cycle clock, if not started yet.
    cycle.update(); // Updates the cycle clock.
    general.button.update(); // Updates general button.
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
    Schedule(dater);
    Schedule(jaw);
    Schedule(photocell);
    Schedule(knife);
    Schedule(cooler);
    Schedule(welder);
    reset();
  } else if (modes.status(STOPPING)) {
    // Shutting down.
  } else if (modes.status(COOLDOWN)) {
    // Not needed yet.
  } else if (modes.status(MAINTENANCE)) {
    // Manually control things.
    // Check everything.
    Serial.begin(9600); // TODO: Put inside a conditional
  }
}

boolean isON(byte pin) {
  if (digitalRead(pin) == ON) {
    return true;
  } else {
    return false;
  }
}

void reset() {
  if (digitalRead(sensor.reset) == !ON) { // If reset sensor is pressed.
    cycle.reset(); // Resets the cycle;
  }
}

void lockAll() {
  vWelder.lock = hWelder.lock = dWelder.lock = general.lock = feeder.lock =
  dater.lock = jaw.lock = photocell.lock = knife.lock = cooler.lock =
  welder.lock = true;
}

void allOFF() {
  digitalWrite(vWelder.out, OFF);
  digitalWrite(hWelder.out, OFF);
  digitalWrite(dWelder.out, OFF);
  digitalWrite(general.out, OFF);
  digitalWrite(feeder.out, OFF);
  digitalWrite(dater.out, OFF);
  digitalWrite(jaw.out, OFF);
  digitalWrite(photocell.out, OFF);
  digitalWrite(knife.out, OFF);
  digitalWrite(cooler.out, OFF);
  digitalWrite(welder.out, OFF);
}

void updateAll() {
  cycle.update();
  general.button.update(); // Updates general button.
}

void Schedule(struct function f) {
  unsigned long start;
  f.start == 0 ? start = 0 : start = (f.start * cycle.last()) / 1500;
  unsigned long stop;
  f.stop == 0 ? stop = 0 : stop = (f.stop * cycle.last()) / 1500;
  if (f.lock == true) { // Locked function
    if (digitalRead(f.out) == ON) { digitalWrite(f.out, OFF); }
  } else { // Unlocked function.
    if (start < stop && BETWEEN(cycle.now(), start, stop)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      digitalWrite(f.out, ON);
    } else if (start > stop && !BETWEEN(cycle.now(), stop, start)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      digitalWrite(f.out, ON);
    } else { if (digitalRead(f.out) == ON) { digitalWrite(f.out, OFF); } }
  }
}

void security() {
  byte s1, s2, s3;
  s1 = isON(sensor.security1);
  s2 = isON(sensor.security2);
  s3 = isON(sensor.security3);
  if (!s1 || !s2 || !s3) {
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
