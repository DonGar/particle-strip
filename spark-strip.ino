/*-------------------------------------------------------------------------
  SparkStrip is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  SparkStrip is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with SparkStrip.  If not, see
  <http://www.gnu.org/licenses/>.

  The original version of SparkStrip is available at:
      https://github.com/DonGar/spark-strip
  -------------------------------------------------------------------------*/

#include "digital_strip.h"
#include "neo_strip.h"
#include "patterns.h"

//
// This is an example of how to use the SparkStrip library.
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

#include "color.h"
#include "strip.h"
#include "patterns.h"

// LPD8806 Strip with 26 LEDs
DigitalStrip stripRgb = DigitalStrip(26);
Pattern stripPattern = Pattern(&stripRgb);

// NeoPixel strip with 16 LEDs
NeoStrip ringRgb = NeoStrip(16, D2, WS2812B);
Pattern ringPattern = Pattern(&ringRgb);

void setup()
{
    stripPattern.setPattern(CYLON, RED, BLACK, 1000);
    ringPattern.setPattern(CYLON, RED, GREEN, 1000);

    Spark.publish("strip", stripPattern.getText(), 60, PRIVATE);
    Spark.publish("ring", ringPattern.getText(), 60, PRIVATE);

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
