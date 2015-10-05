
#include "spark-strip/spark-strip.h"

//
// This is an example of how to use the ParticleStrip library.
//
// For this demo, I used:
//   LPD8806 strip: http://www.adafruit.com/product/306
//   NeoPixel ring: http://www.adafruit.com/products/1463
//
// Both connected, as described in digital_strip.h, and neo_strip.h.
//
// The Spark Core is publishing events describing the patterns being drawn,
//   and is accepting RPC calls to update the patterns at run time.
//

// LPD8806 Strip with 26 LEDs
DigitalStrip stripRgb = DigitalStrip(26);
Pattern stripPattern = Pattern(&stripRgb, "strip");

// NeoPixel strip with 16 LEDs
NeoStrip ringRgb = NeoStrip(16, D2, WS2812B);
Pattern ringPattern = Pattern(&ringRgb, "ring");

void setup()
{
  stripPattern.setPattern(CYLON, RED, BLACK, 1000);
  ringPattern.setPattern(CYLON, RED, GREEN, 1000);

  Spark.function("strip_target", setStripPattern);
  Spark.function("ring_target", setRingPattern);
}

int setStripPattern(String text) {
  int result = stripPattern.setText(text);
  Spark.publish("strip", stripPattern.getText(), 60, PRIVATE);
  return result;
}

int setRingPattern(String text) {
  int result = ringPattern.setText(text);
  Spark.publish("ring", ringPattern.getText(), 60, PRIVATE);
  return result;
}

void loop()
{
  stripPattern.drawUpdate();
  ringPattern.drawUpdate();
}
