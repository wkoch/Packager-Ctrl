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
#define SENSOR INPUT_PULLUP        // INPUT or INPUT_PULLUP
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
  pinMode(general.out, OUT);
  pinMode(dater.out, OUT);
  pinMode(feeder.out, OUT);

  // Start as OFF
  digitalWrite(general.out, OFF);
  digitalWrite(dater.out, OFF);
  digitalWrite(feeder.out, OFF);
}

void loop() {
  if (modes.status(ALARM)) {
    // Turn a led ON?
    // Turn everything else OFF
    // Put Arduino to sleep.
  } else if (modes.status(WARMUP)) {
    delay(3000);
    modes.set(STANDBY);
  } else if (modes.status(STANDBY)) {
    // Buttons work.
    updateAll();
    digitalWrite(feeder.out, feeder.button.state());
    digitalWrite(general.out, general.button.state());
    if (digitalRead(general.out) == ON) {
      modes.set(STARTING);
    }
  } else if (modes.status(STARTING)) {
    // 3 empty cycles.
    updateAll();
    if (digitalRead(feeder.out) == ON) {
      feeder.button.next();
      /*digitalWrite(feeder.out, OFF);*/
      digitalWrite(general.out, general.button.state());
    }
    digitalWrite(general.out, general.button.state());
    if (digitalRead(general.out) == OFF) {
      modes.set(STANDBY);
    }
  } else if (modes.status(PRODUCTION)) {
    // Normal production.
    updateAll();
  } else if (modes.status(STOPPING)) {
    // Shutting down.
    updateAll();
  } else if (modes.status(COOLDOWN)) {
    // Not needed yet.
  } else if (modes.status(MAINTENANCE)) {
    // Manually control things.
    // Check everything.
    Serial.begin(9600); // TODO: Put inside a conditional
  }
}

void updateAll() {
  cycle.update();
  general.button.update();
  dater.button.update();
  feeder.button.update();
}
