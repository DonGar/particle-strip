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

#ifndef LED_STRIP_H
#define LED_STRIP_H

#include "strip.h"

// Implements the ColorStrip interface for a single LED with 3 PWM outputs.

// Can support both common anode or common cathode.
//
// Common Anode RGB LED (most common):
//   Led is lit when the GPIO pins are low (sinking current)
//     Anode - 3.3v (usually longest pin)
//     R/G/B - 200 Ohm resister - any 3 PWM pins.
//
// Common Cathode RGB LED:
//   Led is lit when the GPIO pins are high (sourcing current)
//     Cathode - Ground
//     R/G/B - 200 Ohm resister - any 3 PWN pins.
//
// This strip always has a length of 1, since it only supports a single LED.
// It exists to make it easy to use color mixing/dimming code, as well as
// the simpler animations (pulsing, random colors, etc).
class LedStrip : public ColorStrip   {
  public:
    inline LedStrip(int red_pin, int green_pin, int blue_pin,
                    bool common_anode=true) :
          ColorStrip(1, false),
          red_pin(red_pin), green_pin(green_pin), blue_pin(blue_pin),
          common_anode(common_anode) {

      pinMode(this->red_pin, OUTPUT);
      pinMode(this->green_pin, OUTPUT);
      pinMode(this->blue_pin, OUTPUT);

      drawSolid(BLACK);
    }

    virtual inline void drawPixel(Color color) {
      if (this->drawOffset >= this->pixelCount) {
        return;
      }
      this->drawOffset++;

      if (this->common_anode)
        color = invertColor(color);

      analogWrite(this->red_pin, color.red);
      analogWrite(this->green_pin, color.green);
      analogWrite(this->blue_pin, color.blue);
    }

  private:
    int red_pin, green_pin, blue_pin;
    bool common_anode;
};

#endif
