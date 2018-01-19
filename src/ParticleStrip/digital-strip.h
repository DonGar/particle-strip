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

#ifndef DIGITAL_STRIP_H
#define DIGITAL_STRIP_H

#include "strip.h"

// Implements the ColorStrip interface for a LPD8806 RGB LED strip.

// Example hardware:
//   http://www.adafruit.com/product/306
//
// Uses the standard Particle Core SPI pins.
//
// Strip connections:
//   +5V in to 5V (NOT from the Core, it's too much power draw).
//   Strip CI (clock in) be connected to Core A3.
//   Strip DI (data in) in connected to Core A5.
//   GND to Ground.
class DigitalStrip : public ColorStrip   {
  public:
    inline DigitalStrip(int pixelCount) :
        ColorStrip(pixelCount) {

      SPI.begin();
      SPI.setBitOrder(MSBFIRST);
      SPI.setDataMode(SPI_MODE0);

      this->finishDraw();
      drawSolid(BLACK);
    }

    virtual inline void drawPixel(Color color) {
      ColorStrip::drawPixel(color);

    // GRB color order, oddly, and only 0x7F is sinificant, so
    // through away least significant bit.
      SPI.transfer(color.green >> 1 | 0x80);
      SPI.transfer(color.red >> 1   | 0x80);
      SPI.transfer(color.blue >> 1  | 0x80);
    }

    virtual inline void finishDraw() {
      ColorStrip::finishDraw();

    // Transfer enough zeros to latch all pixel controllers and prepare
    // for new draws.
      int latch_size = ((this->pixelCount+31) / 32) * 8;

      for (int i = 0; i < latch_size; i++) {
        SPI.transfer(0);
      }
    }

};

#endif
