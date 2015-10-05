/*-------------------------------------------------------------------------
  ParticleStrip is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  ParticleStrip is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with ParticleStrip.  If not, see
  <http://www.gnu.org/licenses/>.

  The original version of ParticleStrip is available at:
      'https://github.com/DonGar/particle-strip
  -------------------------------------------------------------------------*/

#ifndef DOT_STRIP_H
#define DOT_STRIP_H

#include "strip.h"

// Implements the ColorStrip interface for a LPD8806 RGB LED strip.

// Example hardware:
//   http://www.adafruit.com/product/306
//
// Uses the standard Core SPI pins, via a level shifter. I used a
//   a 74AHCT125.
//
// Core A3 -> 200 Ohm resister -> 74AHCT125 1A
// Core A5 -> 200 Ohm resister -> 74AHCT125 2A
//
// A high resistence pull up on these might avoid noise at startup.
// 74AHCT125 1OE -> Ground
// 74AHCT125 2OE -> Ground
//
// 74AHCT125 1Y -> Strip CI
// 74AHCT125 2Y -> Strip DI

// Strip +5V in to 5V (NOT from the Spark Core, it's too much power draw).
// Strip GND to Ground.

class DotStrip : public ColorStrip   {
  public:
    DotStrip(int pixelCount);

    virtual void drawPixel(Color color);
    virtual void finishDraw();
};

#endif
