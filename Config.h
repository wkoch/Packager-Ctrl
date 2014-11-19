/*
  Packager-Ctrl - v2.0 - 19/11/2014
  Arduino Due application for packaging machine automation.
  Created by William Koch.
  Released into the public domain.
*/

#include <Debounce.h>
#include <OnePush.h>

struct pwm {
  String name;
  SlowPWM pwm;
  unsigned long cycle;
  byte in, out;
  boolean lock;
};

struct function {
  String name;
  OnePush button;
  byte in, out, led;
  unsigned long start, stop;
  boolean lock;
};

struct sensors {
  byte reset, photocell, jaw, security1, security2, security3;
};


// Configurations
// VERTICAL WELDER PWM
struct pwm vWelder { // Solda Vertical
   .name = "Vertical Welder",
    .pwm = SlowPWM(vWelder.cycle, vWelder.in, vWelder.out),
  .cycle = 1000,
     .in = A0,
    .out = 33,
   .lock = false
};

// HORIZONTAL WELDER PWM
struct pwm hWelder { // Solda Horizontal
   .name = "Horizontal Welder",
    .pwm = SlowPWM(hWelder.cycle, hWelder.in, hWelder.out),
  .cycle = 1000,
     .in = A1,
    .out = 35,
   .lock = false
};

// DATER WELDER PWM
struct pwm dWelder { // Solda Datador
   .name = "Dater Welder",
    .pwm = SlowPWM(dWelder.cycle, dWelder.in, dWelder.out),
  .cycle = 1000,
     .in = A2,
    .out = 37,
   .lock = false
};

// PRODUCTION FUNCTIONS
// GENERAL
struct function general { // Geral
   .name = "General",
 .button = OnePush(general.in),
     .in = 22,
    .out = 23,
    .led = NULL,
  .start = NULL,
   .stop = NULL,
   .lock = false
};

// FEEDER
struct function feeder { // Dosador
   .name = "Feeder",
 .button = OnePush(feeder.in),
     .in = 24,
    .out = 25,
    .led = NULL,
  .start = NULL,
   .stop = NULL,
   .lock = false
};

// DATER
struct function dater { // Datador
   .name = "Dater",
 .button = OnePush(dater.in),
     .in = 26,
    .out = 46,
    .led = 27,
  .start = 0,
   .stop = 500,
   .lock = false
};

// JAW
struct function jaw { // Mandíbula
   .name = "Jaw",
 .button = NULL,
     .in = NULL,
    .out = 40,
    .led = NULL,
  .start = 550,
   .stop = 1500,
   .lock = false
};

// PHOTOCELL
struct function photocell { // Fotocélula
   .name = "Photocell",
 .button = NULL,
     .in = 6,
    .out = NULL,
    .led = NULL,
  .start = 1250,
   .stop = 1400,
   .lock = false
};

// KNIFE
struct function knife { // Faca
   .name = "Knife",
 .button = NULL,
     .in = NULL,
    .out = 42,
    .led = NULL,
  .start = 600,
   .stop = 800,
   .lock = false
};

// COOLER
struct function cooler { // Refrigeração
   .name = "Cooler",
 .button = NULL,
     .in = NULL,
    .out = 44,
    .led = NULL,
  .start = 800,
   .stop = 1450,
   .lock = false
};

// WELDERS
struct function welder { // Solda
   .name = "Welder",
 .button = NULL,
     .in = NULL,
    .out = 48,
    .led = NULL,
  .start = 0,
   .stop = 400,
   .lock = false
};

// SENSORS
struct sensors sensor { // Sensores
      .reset = 28,
  .photocell = photocell.in,
        .jaw = 2,
  .security1 = 32,
  .security2 = 34,
  .security3 = 36
};
