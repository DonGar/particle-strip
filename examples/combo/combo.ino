
#include "particle-strip.h"

//
// This is an example of how to use the ParticleStrip library.
//
// For this demo, I used:
//   LPD8806 strip: http://www.adafruit.com/product/306
//   NeoPixel ring: http://www.adafruit.com/products/1463
//
// Both connected, as described in digital-strip.h, and neo-strip.h.
//
// The Spark Core is publishing events describing the patterns being drawn,
//   and is accepting RPC calls to update the patterns at run time.
//

// LPD8806 Strip with 26 LEDs
DigitalStrip stripRgb(26);
Pattern stripPattern(&stripRgb, "strip");

// NeoPixel strip with 16 LEDs
NeoStrip ringRgb(16, D2, WS2812B);
Pattern ringPattern(&ringRgb, "ring");

int setStripPattern(String text) {
  stripPattern.setPattern(stringToPattern(text));
  return 0;
}

int setRingPattern(String text) {
  ringPattern.setPattern(stringToPattern(text));
  return 0;
}

void setup()
{
  stripPattern.setPattern(TEST, RED, BLACK, 1000);
  ringPattern.setPattern(TEST, RED, GREEN, 1000);

  Spark.function("strip_target", setStripPattern);
  Spark.function("ring_target", setRingPattern);
}

void loop()
{
  if (stripPattern.drawUpdate()) {
    String patternText = patternToString(stripPattern.getPattern());
    Spark.publish("strip", patternText, 60, PRIVATE);
  }

  if (ringPattern.drawUpdate()) {
    String patternText = patternToString(ringPattern.getPattern());
    Spark.publish("ring", patternText, 60, PRIVATE);
  }
}
