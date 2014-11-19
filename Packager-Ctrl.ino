/*
  Packager-Ctrl - v2.0 - 19/11/2014
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
#define WARMUP      0 // Warm-up, a few seconds of delay before Stand-by.
#define STANDBY     1 // Buttons are working.
#define STARTING    2 // Does 3 empty cycles.
#define PRODUCTION  3 // Normal (continuous) production.
#define STOPPING    4 // Finishs current cycle and empties the machine.
#define COOLDOWN    5 // Stops and goes back to Warm-up.
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
#define  UNLOCK(x)        if   (x.lock) {x.lock = false;}
#define    LOCK(x)        if  (!x.lock) {x.lock = true;}
#define ButtonState(x)    if (!x.lock) {digitalWrite(x.out, x.button.state());}
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
  OUT(knife.out);
  OUT(cooler.out);
  OUT(welder.out);
  // Shut all outputs OFF at startup
  allOFF();
}

void loop() {
  security(); // 3 security sensor on the machine.
  if (modes.status(ALARM)) { // ALARM MODE
    cycle.stop();
    lockAll();
    allOFF();
    // TODO: Implement alarm indicator.
  } else if (modes.status(WARMUP)) { // WARMUP MODE
    lockAll();
    allOFF();
    if (cycle.cycles() > 0) {
      cycle.reboot(); // Restarts the cycle counter and clock.
    } else {
      delay(3000);
    }
    modes.set(STANDBY);
  } else if (modes.status(STANDBY)) { // STANDBY MODE
    // Buttons work.
    UNLOCK(general);
    UNLOCK(feeder);
    UNLOCK(dWelder);
    ButtonState(general);
    ButtonState(feeder);
    LedState(dater);
    weldersPWM();
    if (isON(general.out)) { // If general is ON:
      unlockAll();
      modes.set(STARTING); // Change to Starting mode.
    }
  } else if (modes.status(STARTING)) { // STARTING MODE
    // Do 3 empty cycles.
    cycle.start(); // Starts the cycle clock, if not started yet.
    cycle.update(); // Updates the cycle clock.
    LedState(dater); // Dater led via button.
    weldersPWM();
    if (isON(feeder.out)) { // Stops the feeder if working.
      feeder.button.set(0); // Simulate button release.
      ButtonState(feeder); // Turn the feeder OFF.
    }
    ButtonState(general); // Turns general ON via button.
    if (isOFF(general.out)) { // Button pressed to shutdown.
      cycle.stop();
      modes.set(WARMUP); // Shuts down.
    }
    reset();
    if (cycle.cycles() >= 3) { modes.set(PRODUCTION); }
  } else if (modes.status(PRODUCTION)) { // PRODUCTION MODE
    // Normal production.
    ButtonState(feeder); // Feeder ON/OFF via button.
    LedState(dater); // Dater.led ON/OFF via button.
    weldersPWM();
    Schedule(dater);
    Schedule(jaw);
    Schedule(photocell);
    Schedule(knife);
    Schedule(cooler);
    Schedule(welder);
    reset();
    if (cycle.cycles() > 4 && !general.button.status()) {
      if (isON(feeder.out)) { // Shuts down the feeder if ON.
        feeder.button.set(0);
        ButtonState(feeder);
      }
      if (isON(dater.out)) { // Shuts down the dater if ON.
        dater.button.set(0);
        ButtonState(dater);
      }
      modes.set(STOPPING); // Begins soft shutdown.
    }
  } else if (modes.status(STOPPING)) { // STOPPING MODE
    // Soft shutdown.
    weldersPWM();
    // Locks and shuts down Dater and Feeder.
    LOCK(dater); LOCK(dWelder); LOCK(feeder);
    feeder.button.set(0);
    turnOFF(feeder.out);
    dater.button.set(0);
    turnOFF(dater.led);
    turnOFF(dater.out);
    Schedule(jaw);
    Schedule(photocell);
    Schedule(knife);
    Schedule(cooler);
    Schedule(welder);
    reset();
    if (cycle.now() > jaw.stop || cycle.now() == 0) { modes.set(COOLDOWN); }
  } else if (modes.status(COOLDOWN)) { // COOLDOWN MODE
    cycle.stop();
    modes.set(WARMUP);
  } else if (modes.status(MAINTENANCE)) { // MAINTENANCE MODE
    // Manually control things.
    // Check everything individually.
    // Serial.begin(9600); // TODO: Put inside a conditional
    // TODO: Implement when/if needed.
  }
}

void reset() {
  if (isOFF(sensor.reset)) { cycle.reset(); }
  // Undo the lock from the Photocell every reset:
  if (cycle.now() == 0) { UNLOCK(jaw); UNLOCK(knife); UNLOCK(cooler); }
}

void weldersPWM() {
  !vWelder.lock ? vWelder.pwm.on() : vWelder.pwm.off();
  !hWelder.lock ? hWelder.pwm.on() : hWelder.pwm.off();
  (!dWelder.lock && isON(dater.led)) ? dWelder.pwm.on() : dWelder.pwm.off();
}

void lockAll() {
  LOCK(vWelder);
  LOCK(hWelder);
  LOCK(dWelder);
  LOCK(general);
  LOCK(feeder);
  LOCK(dater);
  LOCK(jaw);
  LOCK(photocell);
  LOCK(knife);
  LOCK(cooler);
  LOCK(welder);
}

void unlockAll() {
  UNLOCK(vWelder);
  UNLOCK(hWelder);
  UNLOCK(dWelder);
  UNLOCK(general);
  UNLOCK(feeder);
  UNLOCK(dater);
  UNLOCK(jaw);
  UNLOCK(photocell);
  UNLOCK(knife);
  UNLOCK(cooler);
  UNLOCK(welder);
}

void allOFF() {
  vWelder.pwm.off();
  hWelder.pwm.off();
  dWelder.pwm.off();
  turnOFF(vWelder.out);
  turnOFF(hWelder.out);
  turnOFF(dWelder.out);
  turnOFF(general.out);
  turnOFF(feeder.out);
  turnOFF(dater.out);
  turnOFF(dater.led);
  turnOFF(jaw.out);
  turnOFF(photocell.out);
  turnOFF(knife.out);
  turnOFF(cooler.out);
  turnOFF(welder.out);
}

void updateAll() { cycle.update(); }

void Schedule(struct function f) {
  isON(dater.led) ? UNLOCK(dater) : LOCK(dater);
  if (isOFF(jaw.out)) {   LOCK(knife);   LOCK(cooler);}
  if ( isON(jaw.out)) { UNLOCK(knife); UNLOCK(cooler);}
  unsigned long start;
  f.start == 0 ? start = 0 : start = (f.start * cycle.last()) / jaw.stop;
  unsigned long stop;
  f.stop == 0 ? stop = 0 : stop = (f.stop * cycle.last()) / jaw.stop;
  if (f.lock) { // Locked function?
    turnOFF(f.out);
  } else { // Unlocked function.
    if (start < stop && BETWEEN(cycle.now(), start, stop)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      if (f.name == photocell.name && !jaw.lock && isOFF(sensor.photocell)) {
        LOCK(jaw); LOCK(knife); LOCK(cooler);
      } else {
        turnON(f.out);
      }
    } else if (start > stop && !BETWEEN(cycle.now(), stop, start)) {
      if (f.name == knife.name) { knifeSecurity(); } // Before knife is ON.
      if (f.name == photocell.name && !jaw.lock && isOFF(sensor.photocell)) {
        LOCK(jaw); LOCK(knife); LOCK(cooler);
      } else {
        turnON(f.out);
      }
    } else {
      turnOFF(f.out);
    }
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
