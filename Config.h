/*
  Emp-Ctrl-Due - v2.0a - 14/11/2014
  Arduino Due application for packaging machine automation.
  Created by William Koch.
  Released into the public domain.
*/

#include <Debounce.h>
#include <OnePush.h>

struct pwm {
  String name;
  unsigned long cycle;
  byte in, out;
};

struct function {
  String name;
  OnePush button;
  byte in, out, led;
  unsigned long start, stop;
  boolean lock;
};

struct sensors {
  byte reset, photocell, security1, security2, security3;
};


// Configurations
// VERTICAL WELDER PWM
struct pwm vWelder {
   .name = "Vertical Welder",
  .cycle = 1000,
     .in = A0,
    .out = 33
};

// HORIZONTAL WELDER PWM
struct pwm hWelder {
   .name = "Horizontal Welder",
  .cycle = 1000,
     .in = A1,
    .out = 35
};

// DATER WELDER PWM
struct pwm dWelder {
   .name = "Dater Welder",
  .cycle = 1000,
     .in = A2,
    .out = 37
};

// PRODUCTION FUNCTIONS
// GENERAL
struct function general {
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
struct function feeder {
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
struct function dater {
   .name = "Dater",
 .button = OnePush(dater.in),
     .in = 26,
    .out = 46,
    .led = 27,
  .start = 0,
   .stop = 400,
   .lock = false
};

// JAW
struct function jaw {
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
struct function photocell {
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
struct function knife {
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
struct function cooler {
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
struct function welders {
   .name = "Welders",
 .button = NULL,
     .in = NULL,
    .out = 48,
    .led = NULL,
  .start = 0,
   .stop = 400,
   .lock = false
};

// SENSORS
struct sensors sensor {
      .reset = 2,
  .photocell = photocell.in,
  .security1 = 32,
  .security2 = 34,
  .security3 = 36
};
