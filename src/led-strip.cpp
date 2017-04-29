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

#include "led-strip.h"

const int STRIP_LENGTH = 1;

LedStrip::LedStrip(int red_pin, int green_pin, int blue_pin,
                   bool common_anode) :
    ColorStrip(STRIP_LENGTH, false),
    red_pin(red_pin), green_pin(green_pin), blue_pin(blue_pin),
    common_anode(common_anode) {

  pinMode(this->red_pin, OUTPUT);
  pinMode(this->green_pin, OUTPUT);
  pinMode(this->blue_pin, OUTPUT);

  drawSolid(BLACK);
}

void LedStrip::drawPixel(Color color) {
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
