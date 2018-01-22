
#include "particle-strip.h"

//
// This is an example of how to use various patterns and colors.
//
// For this demo, I used:
//   DotStar LED Strip White 30: https://www.adafruit.com/product/2238
//
// Connected, as described in dot-strip.h.
//

// DotStar strip with 30 LEDs
DotStrip dotRgb(30);  // Replace with any strip type.
Pattern pattern(&dotRgb);

// Switch to a new animation pattern each time it's called.
void advancePattern() {
  static int which = 0;

  switch (which) {
    case 0:
      // TEST pattern to verify your strip control.
      pattern.setPattern(TEST, BLACK, BLACK, 300);
      break;

    case 1:
      // Alternating Christmas colors.
      pattern.setPattern(ALTERNATE, dimColor(RED, 0.25),
                            dimColor(GREEN, 0.25), 500);
      break;

    case 2:
      // Display a solid primary, re-randomize each second.
      pattern.setPattern(SOLID, RANDOM_PRIMARY, BLACK, 1000);
      break;

    case 3:
      // Pulse RANDOM color each pulse. Pulses last 400ms.
      pattern.setPattern(PULSE, RANDOM, BLACK, 10);
      break;

    case 4:
      // Do a red on orange lava lamp behavior.
      Color orange = Color{0x00, 0xFF, 0xA5, 0x00};
      pattern.setPattern(LAVA, RED, orange, 400);
      break;
    // 5, 6 skipped to let LAVA have 3 turns.
  }

  // Use next pattern on next call, wrapping.
  which = (which + 1) % 5;
}

void setup() {
}

void loop() {
  static int start = 0;

  int now = millis();
  if ((now - start) > 20000) {
    // Update the animation pattern ever 20 seconds.
    advancePattern();
    start = now;
  }

  // Redraw strip, with current animation state (if needed).
  pattern.drawUpdate();

  String patternText = patternToString(pattern.getPattern());
}
