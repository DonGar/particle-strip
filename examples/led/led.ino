
#include "particle-strip.h"

//
// This is an example of how to use various patterns and colors.
//
// For this demo, I used:
//   A generic common anode RGB LED.
//   Connect D2, D1, D0 pins to the RGB LED pins via ~330 Ohm resistors.
//     Any three PWM pins are suitable.
//   Connect the anode to 3.3v power.
//

// Common Anode LED with RGB on pins D2, D1, D0.
LedStrip led_strip(D2, D1, D0);
Pattern pattern(&led_strip);

// Switch to a new animation pattern each time it's called.
void advancePattern() {
  static int which = 0;

  switch (which) {
    case 0:
      // TEST pattern to verify primary colors.
      pattern.setPattern(TEST, BLACK, BLACK, 300);
      break;

    case 1:
      // Alternating dim red/green colors.
      pattern.setPattern(ALTERNATE,
                         dimColor(RED, 0.01),
                         dimColor(GREEN, 0.01), 500);
      break;

    case 2:
      // Display a solid primary, re-randomize each second.
      pattern.setPattern(SOLID, RANDOM_PRIMARY, BLACK, 1000);
      break;

    case 4:
      // Pulse RANDOM color each pulse. Pulses last 400ms.
      pattern.setPattern(PULSE, RANDOM, BLACK, 400);
      break;
  }

  // Use next pattern on next call, wrapping.
  which = (which + 1) % 5;
}

void setup() {
}

void loop() {
  static int start = 0;

  int now = millis();
  if ((now - start) > 5000) {
    // Update the animation pattern ever 5 seconds.
    advancePattern();
    start = now;
  }

  // Redraw strip, with current animation state (if needed).
  pattern.drawUpdate();
}
