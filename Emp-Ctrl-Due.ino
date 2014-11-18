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

#define      ON            HIGH
#define     OFF            LOW
#define      IN(x)         pinMode(x, INPUT_PULLUP)
#define  BUTTON(x)         pinMode(x, INPUT_PULLUP)
#define  SENSOR(x)         pinMode(x, INPUT_PULLUP)
#define     OUT(x)         pinMode(x, OUTPUT)
#define BETWEEN(x, a, b)  ((a) <= (x) && (x) <= (b))
#define    isON(x)        (digitalRead(x) == ON)
#define   isOFF(x)        (digitalRead(x) == OFF)
#define  turnON(x)        if (isOFF(x)) {digitalWrite(x, ON);}
#define turnOFF(x)        if  (isON(x)) {digitalWrite(x, OFF);}
#define ButtonState(x)    digitalWrite(x.out, x.button.state())
#define    LedState(x)    digitalWrite(x.led, x.button.state())

Modus modes(7);
Cyclic cycle(2000, 900);

void setup() {
  // BUTTONS
  BUTTON(general.in);
  BUTTON(dater.in);
  BUTTON(feeder.in);
  // SENSORS
  SENSOR(sensor.reset);
  SENSOR(sensor.photocell);
  SENSOR(sensor.jaw);
  SENSOR(sensor.security1);
  SENSOR(sensor.security2);
  SENSOR(sensor.security3);

  // OUTPUTS
  OUT(vWelder.out);
  OUT(hWelder.out);
  OUT(dWelder.out);
  OUT(general.out);
  OUT(feeder.out);
  OUT(dater.out);
  OUT(dater.led);
  OUT(jaw.out);
  OUT(photocell.out);
  OUT(knife.out);
  OUT(cooler.out);
  OUT(welder.out);

  // Shut all outputs OFF at startup
  allOFF();
}

void loop() {
  security();
  if (modes.status(ALARM)) {
    cycle.stop();
    lockAll();
    allOFF();
  } else if (modes.status(WARMUP)) {
    lockAll();
    allOFF();
    delay(3000);
    modes.set(STANDBY);
  } else if (modes.status(STANDBY)) {
    lockAll();
    allOFF();
    // Buttons work.
    ButtonState(feeder);
    ButtonState(general);
    if (isON(general.out)) { // If general is ON:
      unlockAll();
      modes.set(STARTING); // Change to Starting mode.
    }
  } else if (modes.status(STARTING)) {
    // 3 empty cycles.
    cycle.start(); // Starts the cycle clock, if not started yet.
    cycle.update(); // Updates the cycle clock.
    weldersPWM();
    if (isON(feeder.out)) { // If the feeder is ON:
      feeder.button.next(); // Simulate button press and.
      ButtonState(feeder); // Turn the feeder OFF.
    }
    ButtonState(general); // Turns general ON.
    if (isOFF(general.out)) { // If general button is pressed.
      cycle.stop();
      modes.set(STANDBY); // Shuts down again and changes to Starting mode.
    }
    reset();
    if (cycle.cycles() >= 3) { // If 3 or more cycles were completed:
      modes.set(PRODUCTION); // Changes to Prodution mode.
    }
  } else if (modes.status(PRODUCTION)) {
    // Normal production.
    ButtonState(feeder);
    LedState(dater);

    weldersPWM();
    Schedule(dater);
    Schedule(jaw);
    Schedule(photocell);
    Schedule(knife);
    Schedule(cooler);
    Schedule(welder);
    reset();
    if (cycle.cycles() > 4 && !general.button.status()) {
      if (isON(feeder.out)) { // If the feeder is ON:
        feeder.button.next(); // Simulate button press and
        ButtonState(feeder); // Turn the feeder OFF.
      }
      if (isON(dater.out)) { // If the dater is ON:
        dater.button.next(); // Simulate button press and
        ButtonState(dater); // Turn the dater OFF.
      }
      modes.set(STOPPING); // Begins soft shutdown.
    }
  } else if (modes.status(STOPPING)) {
    // Shutting down.
    weldersPWM();
    if (isON(dater.out) || !dater.lock) {
      dater.lock = true;
      turnOFF(dater.out);
    }
    if (!feeder.lock) {
      feeder.lock = true;
      turnOFF(dater.out);
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
    turnON(cooler.out);
    delay(1000);
    turnOFF(cooler.out);
    cycle.reboot(); // Restarts the cycle counter and clock.
    modes.set(STANDBY);
  } else if (modes.status(MAINTENANCE)) {
    // Manually control things.
    // Check everything individually.
    Serial.begin(9600); // TODO: Put inside a conditional
  }
}


void reset() { if (isOFF(sensor.reset)) { cycle.reset(); } }

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
  turnOFF(vWelder.out);
  turnOFF(hWelder.out);
  turnOFF(dWelder.out);
  if (!modes.status(STANDBY)) {
    turnOFF(general.out);
    turnOFF(feeder.out);
    turnOFF(dater.led);
  }
  turnOFF(dater.out);
  turnOFF(jaw.out);
  turnOFF(photocell.out);
  turnOFF(knife.out);
  turnOFF(cooler.out);
  turnOFF(welder.out);
}

void updateAll() { cycle.update(); }

void Schedule(struct function f) {
  isON(dater.led) ? dater.lock = false : dater.lock = true;
  unsigned long start;
  f.start == 0 ? start = 0 : start = (f.start * cycle.last()) / jaw.stop;
  unsigned long stop;
  f.stop == 0 ? stop = 0 : stop = (f.stop * cycle.last()) / jaw.stop;
  if (f.lock) { // Locked function?
    if (isON(f.out)) { turnOFF(f.out); }
  } else { // Unlocked function.
    if (start < stop && BETWEEN(cycle.now(), start, stop)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      turnON(f.out);
    } else if (start > stop && !BETWEEN(cycle.now(), stop, start)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      turnON(f.out);
    } else { if (isON(f.out)) { turnOFF(f.out); } }
  }
}

void security() {
  byte s1 = isON(sensor.security1);
  byte s2 = isON(sensor.security2);
  byte s3 = isON(sensor.security3);
  if (!s1 || !s2 || !s3) { // Negated because of INPUT_PULLUP.
    if (!modes.status(ALARM)) { modes.set(ALARM); }
  }
}

void knifeSecurity() {
  if (isON(sensor.jaw) && isON(jaw.out)) { modes.set(ALARM); }
}
